#include "Header Files/PlayerSprite.hpp"
#include <iostream>
const float fallSpeed = 981.0f;
const float jumplength= -350.0f;
const float frameChangeRate = 0.1f; 

PlayerSprite::PlayerSprite(float x, float y) {
    playerTexture.loadFromFile("img/sprite sheet-grayscale.png");
    playerSprite.setTexture(playerTexture);

    frameWidth = 256;  // 1024 / 4 frames
    frameHeight = 256; //
    frameCount = 4;
    animationTimer = 0.0f;

    currentFrameRect = sf::IntRect(0, 0, frameWidth, frameHeight); // <--
    playerSprite.setTextureRect(currentFrameRect);                 
    playerSprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f); 
    playerSprite.setScale(0.25f, 0.25f);

    position = sf::Vector2f(x, y);
    color = sf::Color(255, 0, 255);
    isActive = true;
    velocity = sf::Vector2f(0.0f, 0.0f);
    health = 3;

    playerSprite.setPosition(position);   
    playerSprite.setColor(color);      
}

void PlayerSprite::update(float dt) {
    if (!isActive) return;

    velocity.y += fallSpeed * dt;
    position.y += velocity.y * dt;
        playerSprite.setPosition(position); // <--

    if (velocity.y < 0) { 
        
        animationTimer += dt;
         if (animationTimer >= frameChangeRate) {
            animationTimer -= frameChangeRate; // Reset timer
            int currentFrame = currentFrameRect.left / frameWidth;
            currentFrame = (currentFrame + 1) % frameCount;
            currentFrameRect.left = currentFrame * frameWidth;
        }
    } else {        currentFrameRect.left = 0;
        animationTimer = 0; 
    }

    playerSprite.setTextureRect(currentFrameRect); // <--
}

void PlayerSprite::Draw(sf::RenderWindow& window) {
    if (!isActive) return;
    
    window.draw(playerSprite); 
}


void PlayerSprite::jump() {
    if (isActive) {
        velocity.y =jumplength;
    }
}

void PlayerSprite::changeColor(sf::Color newColor) {
    this->color = newColor;
    playerSprite.setColor(this->color); 
}

void PlayerSprite::onCollision() {
    //health --
}
