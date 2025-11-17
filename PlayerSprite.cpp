#include "Header Files/PlayerSprite.hpp"
#include <iostream> 

// physics constants
const float GRAVITY = 981.0f; // pixels per second squared
const float JUMP_STRENGTH = -350.0f; // negative y-velocity
const float ANIMATION_SPEED = 0.1f; // seconds per frame

PlayerSprite::PlayerSprite(float x, float y) {
    // load the player's art from the img folder
    if (!playerTexture.loadFromFile("img/sprite sheet-grayscale.png")) {
        std::cerr << "Error: Could not load 'img/sprite sheet-grayscale.png'" << std::endl;
    }
    playerSprite.setTexture(playerTexture);

    // set up animation frame info
    frameWidth = 256;  // 1024px width / 4 frames
    frameHeight = 256;
    frameCount = 4;
    animationTimer = 0.0f;

    // set the first frame
    currentFrameRect = sf::IntRect(0, 0, frameWidth, frameHeight);
    playerSprite.setTextureRect(currentFrameRect);

    // set the sprite's origin to its center
    playerSprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    playerSprite.setScale(0.25f, 0.25f); // scale it down to 64x64

    // set initial properties from Element base class
    position = sf::Vector2f(x, y);
    color = sf::Color(255, 0, 255); // start as bright pink
    isActive = true;

    // set initial player physics
    velocity = sf::Vector2f(0.0f, 0.0f);
    health = 3;

    // apply position and tint to the visible sprite
    playerSprite.setPosition(position);
    playerSprite.setColor(color);
}

void PlayerSprite::update(float dt) {
    if (!isActive) return;

    // --- 1. physics ---
    velocity.y += GRAVITY * dt;     // apply gravity
    position.y += velocity.y * dt;  // apply velocity to position
    playerSprite.setPosition(position); // update the visual sprite

    // --- 2. animation ---
    if (velocity.y < 0) { // if moving up (jumping)
        animationTimer += dt;
        
        if (animationTimer >= ANIMATION_SPEED) {
            animationTimer -= ANIMATION_SPEED; // reset timer

            // move to the next frame
            int currentFrame = currentFrameRect.left / frameWidth;
            currentFrame = (currentFrame + 1) % frameCount;
            currentFrameRect.left = currentFrame * frameWidth;
        }
    } else {
        // not jumping, so use the first frame (idle/falling)
        currentFrameRect.left = 0;
        animationTimer = 0; 
    }

    playerSprite.setTextureRect(currentFrameRect); // apply the correct frame
}

void PlayerSprite::Draw(sf::RenderWindow& window) {
    if (!isActive) return;
    window.draw(playerSprite);
}

void PlayerSprite::jump() {
    if (isActive) {
        velocity.y = JUMP_STRENGTH; // give a single upward boost
    }
}

void PlayerSprite::changeColor(sf::Color newColor) {
    this->color = newColor;
    playerSprite.setColor(this->color); // apply tint to the sprite
}

void PlayerSprite::onCollision() {
    // we'll add this later (e.g., player hit an obstacle)
}

// returns the player's logical center position
sf::Vector2f PlayerSprite::getPosition() const {
    return this->position;
}

// returns the sprite's visible collision box
sf::FloatRect PlayerSprite::getBounds() const {
    return playerSprite.getGlobalBounds();
}