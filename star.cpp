#include "Header Files/star.hpp" // use the correct path
#include <iostream>

Star::Star(float x, float y) {
    // load the star texture from the img folder
    if (!starTexture.loadFromFile("img/star.png")) {
        std::cerr << "error: could not load star.png" << std::endl;
    }
    starSprite.setTexture(starTexture);
    
    // set origin to the center of the image
    starSprite.setOrigin(starTexture.getSize().x / 2.f, starTexture.getSize().y / 2.f);
    
    position = sf::Vector2f(x, y);
    starSprite.setPosition(position);
    starSprite.setScale(0.15f, 0.15f); // 256px -> ~38px
    
    collected = false;
    isActive = true;
}

void Star::update(float dt) {
    // stars don't move, so we only check if we should be active
    if (!isActive || collected) return;
}

void Star::Draw(sf::RenderWindow& window) {
    // don't draw the star if it's already been collected
    if (!isActive || collected) return; 
    window.draw(starSprite);
}

bool Star::isCollected() const {
    return collected;
}

void Star::collect() {
    collected = true;
}

// returns the star's visible collision box
sf::FloatRect Star::getBounds() const {
    return starSprite.getGlobalBounds();
}