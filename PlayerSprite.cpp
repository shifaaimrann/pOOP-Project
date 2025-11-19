#include "Header Files/PlayerSprite.hpp"
#include <iostream>
#include <cmath> // for sin

const float fallStrength = 981.0f; 
const float jumpStrength = -350.0f; 
const float ANIMATION_SPEED = 0.1f; 

// helper for rainbow color
sf::Color getRainbow(float t) {
    float r = std::sin(t * 2.0f) * 0.5f + 0.5f;
    float g = std::sin(t * 2.0f + 2.0f * 3.14159f / 3.0f) * 0.5f + 0.5f;
    float b = std::sin(t * 2.0f + 4.0f * 3.14159f / 3.0f) * 0.5f + 0.5f;
    return sf::Color(r * 255, g * 255, b * 255);
}

PlayerSprite::PlayerSprite(float x, float y) {
    if (!playerTexture.loadFromFile("img/sprite sheet-grayscale.png")) {
        std::cerr << "error loading sprite sheet" << std::endl;
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
    color = sf::Color(255, 0, 255); 
    isActive = true;

    velocity = sf::Vector2f(0.0f, 0.0f);
    health = 100.0f; 
    score = 0;

    // particle init
    particleSpawnTimer = 0.0f;
    totalTime = 0.0f;

    playerSprite.setPosition(position);
    playerSprite.setColor(color);
}

void PlayerSprite::update(float dt) {
    if (!isActive) return;

    // 1. physics
    velocity.y += fallStrength * dt;     
    position.y += velocity.y * dt;  
    playerSprite.setPosition(position); 

    // 2. animation
    if (velocity.y < 0) { 
        animationTimer += dt;
        if (animationTimer >= ANIMATION_SPEED) {
            animationTimer -= ANIMATION_SPEED; 
            int currentFrame = currentFrameRect.left / frameWidth;
            currentFrame = (currentFrame + 1) % frameCount;
            currentFrameRect.left = currentFrame * frameWidth;
        }
    } else {
        currentFrameRect.left = 0;
        animationTimer = 0; 
    }
    playerSprite.setTextureRect(currentFrameRect); 

    // 3. particle system logic
    totalTime += dt;
    particleSpawnTimer += dt;

    // spawn new particles
    if (particleSpawnTimer > 0.02f) {
        particleSpawnTimer = 0.0f;
        Particle p;
        p.position = position; // start at player center
        
        // little random wiggle
        float wiggle = (std::rand() % 100) / 100.0f - 0.5f; 
        p.velocity = sf::Vector2f(wiggle * 50.f, 20.f); 
        p.lifetime = sf::seconds(0.7f); 
        p.shape.setSize(sf::Vector2f(10, 10));
        p.shape.setOrigin(5, 5);
        p.shape.setPosition(p.position);
        p.color = getRainbow(totalTime);
        p.shape.setFillColor(p.color);
        
        particles.push_back(p);
    }

    // update existing particles
    for (int i = particles.size() - 1; i >= 0; --i) {
        particles[i].lifetime -= sf::seconds(dt);
        if (particles[i].lifetime <= sf::Time::Zero) {
            particles.erase(particles.begin() + i);
        } else {
            particles[i].position += particles[i].velocity * dt;
            particles[i].shape.setPosition(particles[i].position);
            
            // fade out
            float lifeRatio = particles[i].lifetime.asSeconds() / 0.7f;
            sf::Color c = particles[i].color;
            c.a = static_cast<sf::Uint8>(lifeRatio * 255);
            particles[i].shape.setFillColor(c);
        }
    }
}

void PlayerSprite::Draw(sf::RenderWindow& window) {
    if (!isActive) return;
    
    // draw particles first (behind player)
    for (const auto& p : particles) {
        window.draw(p.shape);
    }
    
    // draw player on top
    window.draw(playerSprite);
}

void PlayerSprite::jump() {
    if (isActive) velocity.y = jumpStrength; 
}

void PlayerSprite::changeColor(sf::Color newColor) {
    color = newColor;
    playerSprite.setColor(color);
}

void PlayerSprite::onCollision(const std::string& type) {
    if (type == "Obstacle") {
        health -= 30.0f; 
        velocity.y = 150.0f; // bounce
        if (health < 0) health = 0;
    }
    else if (type == "Star") {
        score++;
        health += 20.0f;
        if (health > 100.0f) health = 100.0f;
    }
}

sf::Vector2f PlayerSprite::getPosition() const { return position; }
sf::FloatRect PlayerSprite::getBounds() const { return playerSprite.getGlobalBounds(); }
int PlayerSprite::getScore() const { return score; }
float PlayerSprite::getHealth() const { return health; }