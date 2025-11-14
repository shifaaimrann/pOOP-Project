#include "Header Files/PlayerSprite.hpp" 


const float fallBackStrength = 981.0f;//bascally gravity
const float JUMP_STRENGTH = -350.0f;
PlayerSprite::PlayerSprite(float x, float y) {
    //-------------placeholder
    radius = 20.0f;
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    position = sf::Vector2f(x, y);
    color = sf::Color::White;
    isActive = true;
    velocity = sf::Vector2f(0.0f, 0.0f);
    health = 3; 
    shape.setPosition(position);
    shape.setFillColor(color);
}
void PlayerSprite::update(float dt) {
    if (!isActive) return;
    velocity.y += fallBackStrength * dt;
    position.y += velocity.y * dt;
    shape.setPosition(position);

    //animation logic!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
void PlayerSprite::Draw(sf::RenderWindow& window) {
    if (!isActive) return;
    
    window.draw(shape);
}
void PlayerSprite::jump() {
    if (isActive) {
        velocity.y = JUMP_STRENGTH;
    }
}
void PlayerSprite::changeColor(sf::Color newColor) {
    color = newColor; 
    shape.setFillColor(color);
}

void PlayerSprite::onCollision() {
   
    // health--;
    // playSound("bonk.wav");
}