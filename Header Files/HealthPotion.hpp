#pragma once
#include "Element.hpp"
#include <SFML/Graphics.hpp>

class HealthPotion : public Element {
public:
    HealthPotion(float x, float y) {
       
        
        sprite.setTexture(texture);
        sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
        sprite.setScale(0.4f, 0.4f); // Adjust scale as needed
        
        position = sf::Vector2f(x, y);
        sprite.setPosition(position);
        
        color = sf::Color::White;
        isActive = true;
        isCollected = false;
        
        // Timing
        lifeTimer = 0.f;
        maxLifetime = 30.f; // 30 seconds
        
        // Blinking
        blinkTimer = 0.f;
        blinkInterval = 0.3f; // Blink every 0.3 seconds
        isVisible = true;
    }

    void update(float dt) override {
        if (!isActive || isCollected) return;

        // Update lifetime
        lifeTimer += dt;
        if (lifeTimer >= maxLifetime) {
            isActive = false; // Potion expires
            return;
        }

        // Blinking effect
        blinkTimer += dt;
        if (blinkTimer >= blinkInterval) {
            blinkTimer = 0.f;
            isVisible = !isVisible;
            
            // Set transparency based on visibility
            sf::Color c = sprite.getColor();
            c.a = isVisible ? 255 : 80; // Fully visible or faded
            sprite.setColor(c);
        }
    }

    void Draw(sf::RenderWindow& window) override {
        if (isActive && !isCollected) {
            window.draw(sprite);
        }
    }

    sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }

    void collect() {isCollected = true; isActive = false;}

    bool isExpired() const {return lifeTimer >= maxLifetime;}

    bool hasBeenCollected() const {return isCollected;}

    float getHealAmount() const {return 30.f; }

private:
    sf::Texture texture;
    sf::Sprite sprite;
    
    bool isCollected;
    bool isVisible;
    
    float lifeTimer;
    float maxLifetime;
    
    float blinkTimer;
    float blinkInterval;
};