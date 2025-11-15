#include "PlayerSprite.hpp"
#include <iostream>
const float fallSpeed = 981.0f;
const float jumplength= -350.0f;
const float frameChangeRate = 0.1f; 

PlayerSprite::PlayerSprite(float x, float y) {
    playerTexture.loadFromFile("img/sprite sheet-grayscale.png");
    playerSprite.setTexture(playerTexture); // <-- Apply the texture to the sprite

    // Set up animation frames
    frameWidth = 256;  // 1024 / 4 frames
    frameHeight = 256; //
    frameCount = 4;
    animationTimer = 0.0f;

    // Start at the first frame (rect starts at 0,0 and is 256x256)
    currentFrameRect = sf::IntRect(0, 0, frameWidth, frameHeight); // <--
    playerSprite.setTextureRect(currentFrameRect);                 // <--

    // Set origin to the center of the 256x256 frame
    playerSprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f); // <--

    // --- ADD THIS LINE TO SCALE THE SPRITE ---
    // 0.25f makes it 64x64 pixels. 
    // Try 0.125f (for 32x32) or 0.5f (for 128x128)
    playerSprite.setScale(0.25f, 0.25f);

    // Set starting values from the base 'Element' class
    position = sf::Vector2f(x, y);
    color = sf::Color(255, 0, 255); // Start as bright pink
    isActive = true;

    // Set player-specific values
    velocity = sf::Vector2f(0.0f, 0.0f);
    health = 3;

    // Apply the initial position and color (tint)
    playerSprite.setPosition(position);   // <--
    playerSprite.setColor(color);         // <-- This is the tint!
}

// --- 2. The Update Function ---
void PlayerSprite::update(float dt) {
    if (!isActive) return;

    // --- Physics (same as before) ---
    velocity.y += fallSpeed * dt;
    position.y += velocity.y * dt;
    
    // Update the visual sprite's position
    playerSprite.setPosition(position); // <--

    // --- New Animation Logic ---
    if (velocity.y < 0) { // Check if we are moving UP (jumping)
        
        // Add time to our animation timer
        animationTimer += dt;
        
        // If enough time has passed, change the frame
        if (animationTimer >= frameChangeRate) {
            animationTimer -= frameChangeRate; // Reset timer

            // Get the current frame number (0, 1, 2, or 3)
            int currentFrame = currentFrameRect.left / frameWidth;
            
            // Go to the next frame, looping back to 0
            currentFrame = (currentFrame + 1) % frameCount;
            
            // Set the new frame's horizontal position
            currentFrameRect.left = currentFrame * frameWidth;
        }
    } else {
        // Not jumping, so reset to the first frame (idle/falling)
        currentFrameRect.left = 0;
        animationTimer = 0; // Reset timer so it's ready for the next jump
    }

    // Apply the new texture rect to the sprite
    playerSprite.setTextureRect(currentFrameRect); // <--
}

// --- 3. The Draw Function ---
void PlayerSprite::Draw(sf::RenderWindow& window) {
    if (!isActive) return;
    
    window.draw(playerSprite); // <-- Draw the sprite, not the shape
}

// --- 4. The Jump Function ---
void PlayerSprite::jump() {
    if (isActive) {
        velocity.y =jumplength;
    }
    // (This function doesn't need to change at all)
}

// --- 5. The Change Color Function ---
void PlayerSprite::changeColor(sf::Color newColor) {
    this->color = newColor;
    playerSprite.setColor(this->color); // <-- Tint the sprite, don't fill a shape
}

// --- 6. The Collision Function ---
void PlayerSprite::onCollision() {
    // (No changes needed yet)
}
