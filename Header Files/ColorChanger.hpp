#pragma once
#include "Element.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

class ColorChanger : public Element {
public:
    ColorChanger(float x, float y) {
        if (!texture.loadFromFile("img/colorchange.png")) {
            // Handle error
            // cout<<"Error loading from file"<<endl;
        }
        sprite.setTexture(texture);
        
        // Center origin
        sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
        sprite.setScale(0.4f, 0.4f); // Adjust scale to fit your game
        
        position = sf::Vector2f(x, y);
        sprite.setPosition(position);
        
        isActive = true;
        isCollected = false;
    }

    void update(float dt) override {
        if (!isActive || isCollected) return;
        sprite.setPosition(position); // Sync position
    }

    void Draw(sf::RenderWindow& window) override {
        if (isActive && !isCollected) {
            window.draw(sprite);
        }
    }

    sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }

    bool isActiveItem() const { return isActive && !isCollected; }
    
    void collect() {
        isCollected = true;
        isActive = false; 
    }

private:
    sf::Texture texture;
    sf::Sprite sprite;
    bool isCollected;
};