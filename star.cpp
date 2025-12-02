#include "Header Files/star.hpp"
#include <iostream>

Star::Star(float x, float y) {
    if (!starTexture.loadFromFile("img/star.png")) {
        std::cerr << "error: could not load star.png" << std::endl;
    }
    starSprite.setTexture(starTexture);
    starSprite.setOrigin(starTexture.getSize().x / 2.f, starTexture.getSize().y / 2.f);
    
    position = sf::Vector2f(x, y);
    starSprite.setPosition(position);
    starSprite.setScale(0.15f, 0.15f); 
    
    collected = false;
    isActive = true;
}

void Star::update(float dt) {
    if (!isActive || collected) return;

    // Sync the visual sprite with the logical position
    starSprite.setPosition(position);
}

void Star::Draw(sf::RenderWindow& window) {
    if (!isActive || collected) return; 
    window.draw(starSprite);
}

bool Star::isCollected() const {
    return collected;
}

void Star::collect() {
    collected = true;
}

sf::FloatRect Star::getBounds() const {
    return starSprite.getGlobalBounds();
}