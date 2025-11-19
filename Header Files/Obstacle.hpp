#pragma once
#include "Element.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class Obstacle : public Element {
public:
    virtual void Draw(sf::RenderWindow& window) = 0;
    virtual void update(float dt) = 0;

    // --- NEW: Each obstacle must decide if it hit the player ---
    // Returns TRUE if the player hit the wrong color (Game Over condition)
    virtual bool checkCollision(sf::FloatRect playerBounds, sf::Color playerColor, sf::Vector2f playerPos) const = 0;

    // Helper to check simple overlap
    virtual sf::FloatRect getBounds() const = 0;

protected:
    std::vector<double> size;
};

/// Paddle Class (Updated)
class Paddle : public Obstacle {
public:
    Paddle(float x, float y, float width, float height) {
        position = sf::Vector2f(x, y);
        isActive = true;
        
        paddleShape.setSize(sf::Vector2f(width, height));
        paddleShape.setOrigin(width / 2.f, height / 2.f);
        paddleShape.setPosition(position);
        
        // Define the exact colors the player can have
        validColors = {
            sf::Color::Red,
            sf::Color::Blue,
            sf::Color::Yellow,
            sf::Color::Magenta // "Pink"
        };
        
        colorIndex = 0;
        color = validColors[colorIndex];
        switchTimer = 0.0f;
        switchInterval = 2.0f; // Change color every 2 seconds (slower!)
        
        paddleShape.setFillColor(color);
    }

    void update(float dt) override {
        // 1. Color Switching Logic
        switchTimer += dt;
        if (switchTimer >= switchInterval) {
            switchTimer = 0.0f;
            colorIndex = (colorIndex + 1) % validColors.size();
            color = validColors[colorIndex];
            paddleShape.setFillColor(color);
        }

        // 2. Sync Position (CRITICAL for scrolling)
        // If the level moves the 'Element::position', we must update the shape
        paddleShape.setPosition(position); 
    }

    void Draw(sf::RenderWindow& window) override {
        window.draw(paddleShape);
    }

    sf::FloatRect getBounds() const override {
        return paddleShape.getGlobalBounds();
    }

    bool checkCollision(sf::FloatRect playerBounds, sf::Color playerColor, sf::Vector2f playerPos) const override {
        if (playerBounds.intersects(paddleShape.getGlobalBounds())) {
            if (playerColor != this->color) {
                return true; // Wrong color!
            }
        }
        return false;
    }

private:
    sf::RectangleShape paddleShape;
    std::vector<sf::Color> validColors;
    int colorIndex;
    float switchTimer;
    float switchInterval;
};