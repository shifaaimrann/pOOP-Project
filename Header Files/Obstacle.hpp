#pragma once
#include "Element.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class Obstacle : public Element {
public:
    virtual void Draw(sf::RenderWindow& window) = 0;
    virtual void update(float dt) = 0;
    virtual float getDamage() const = 0;
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

    float getDamage() const override { 
        return 20.0f; 
    }

private:
    sf::RectangleShape paddleShape;
    std::vector<sf::Color> validColors;
    int colorIndex;
    float switchTimer;
    float switchInterval;
};


class LaserBeam : public Obstacle {
public:
    LaserBeam(float x, float y, float width, float height, float speed = 0.f)
    {
        position = {x, y};
        isActive = true;

        beam.setSize({width, height});
        beam.setOrigin(width / 2.f, height / 2.f);
        beam.setPosition(position);
        beam.setFillColor(sf::Color(255, 50, 50));   // red laser

        this->speed = speed; // if you want it to move horizontally
    }

    void update(float dt) override {
        // OPTIONAL: move left-right
        if (speed != 0.f) {
            position.x += speed * dt;

            // bounce from edges
            if (position.x < 50 || position.x > 750)
                speed = -speed;
        }

        beam.setPosition(position);
    }

    void Draw(sf::RenderWindow& window) override {
        window.draw(beam);
    }

    sf::FloatRect getBounds() const override {
        return beam.getGlobalBounds();
    }

    // Simple: if intersect → damage
    bool checkCollision(sf::FloatRect playerBounds,
                        sf::Color playerColor,
                        sf::Vector2f playerPos) const override
    {
        return playerBounds.intersects(beam.getGlobalBounds());
    }

    float getDamage() const override {
        return 15.0f;  // laser deals medium damage
    }

private:
    sf::RectangleShape beam;
    float speed;
};

class ColorSquare : public Obstacle {
public:
    ColorSquare(float x, float y, float sideLength, float thickness, float rotationSpeed);

    void update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

    float getDamage() const override { return 1.0f; }

    bool checkCollision(sf::FloatRect playerBounds, sf::Color playerColor, sf::Vector2f playerPos) const override;

    sf::FloatRect getBounds() const override;

private:
    sf::Color getColorAtSide(int side) const;

private:
    float m_size;
    float m_thickness;
    float m_rotation;
    float m_rotationSpeed;

    sf::VertexArray m_vertices;
    sf::Color m_colors[4]; // Colors for 4 sides
};

class ColorSpotWheel : public Obstacle {
public:
    ColorSpotWheel(float x, float y, float radius, float spotRadius, int spotCount, float rotationSpeed);

    void update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

    float getDamage() const override { return 1.0f; }
    bool checkCollision(sf::FloatRect playerBounds, sf::Color playerColor, sf::Vector2f playerPos) const override;

    sf::FloatRect getBounds() const override;

private:
    float m_radius;
    float m_spotRadius;
    float m_rotationSpeed;
    float m_rotation;
    std::vector<sf::Color> m_spotColors;  // pre-assigned colors for each spot
    int m_spotCount;

    sf::CircleShape m_spot;  // reused for drawing

    sf::Color m_colors[4];

    sf::Color getColorAtAngle(float angle) const;
};

class ColorCross : public Obstacle {
public:
    ColorCross(float x, float y, float armLength, float thickness, float rotationSpeed);

    void update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

    float getDamage() const override { return 1.0f; }
    bool checkCollision(sf::FloatRect playerBounds, sf::Color playerColor, sf::Vector2f playerPos) const override;

    sf::FloatRect getBounds() const override;

private:
    float m_armLength;
    float m_thickness;
    float m_rotationSpeed;
    float m_rotation;

    sf::RectangleShape m_arm1; // vertical
    sf::RectangleShape m_arm2; // horizontal

    sf::Color m_colors[4]; // Pink, Blue, Yellow, Red

    sf::Color getColorAtPoint(sf::Vector2f localPos) const;
};
