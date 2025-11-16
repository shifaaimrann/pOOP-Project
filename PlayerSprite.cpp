#include "Header Files/PlayerSprite.hpp"
#include <iostream>
const float fallStrength = 981.0f;
const float jumpStrength = -350.0f;
const float ANIMATION_SPEED = 0.1f;

PlayerSprite::PlayerSprite(float x, float y) {
    if (!playerTexture.loadFromFile("img/sprite sheet-grayscale.png")) {
        std::cerr << "Error: Could not load player texture!" << std::endl;
    }
    playerSprite.setTexture(playerTexture);

    frameWidth = 256;
    frameHeight = 256;
    frameCount = 4;
    animationTimer = 0.0f;

    currentFrameRect = sf::IntRect(0, 0, frameWidth, frameHeight);
    playerSprite.setTextureRect(currentFrameRect);

    playerSprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    playerSprite.setScale(0.25f, 0.25f);

    position = sf::Vector2f(x, y);
    color = sf::Color(255, 0, 255); //start as bright pink
    isActive = true;

    velocity = sf::Vector2f(0.0f, 0.0f);
    health = 3;

    playerSprite.setPosition(position);
    playerSprite.setColor(color);
}

void PlayerSprite::update(float dt) {
    if (!isActive) return;

    velocity.y += fallStrength * dt;
    position.y += velocity.y * dt;
    playerSprite.setPosition(position);

    if (velocity.y < 0) { //check if we are moving up (jumping)
        animationTimer += dt;
        if (animationTimer >= ANIMATION_SPEED) {
            animationTimer -= ANIMATION_SPEED;
            int currentFrame = currentFrameRect.left / frameWidth;
            currentFrame = (currentFrame + 1) % frameCount;
            currentFrameRect.left = currentFrame * frameWidth;
        }
    } else {
        currentFrameRect.left = 0; //not jumping, so reset to the first frame
        animationTimer = 0;
    }
    playerSprite.setTextureRect(currentFrameRect);
}

void PlayerSprite::Draw(sf::RenderWindow& window) {
    if (!isActive) return;
    window.draw(playerSprite);
}

void PlayerSprite::jump() {
    if (isActive) {
        velocity.y = jumpStrength;
    }
}

void PlayerSprite::changeColor(sf::Color newColor) {
    this->color = newColor;
    playerSprite.setColor(this->color);
}

void PlayerSprite::onCollision() {
    //baad mai akrenge
}

sf::Vector2f PlayerSprite::getPosition() const {
    return this->position;
}