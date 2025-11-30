#include "Header Files/PlayerSprite.hpp"
#include <iostream>
#include <cmath> // for sin

const float fallStrength = 981.0f; 
const float jumpStrength = -350.0f; 
const float ANIMATION_SPEED = 0.1f; 

sf::Color getRainbow(float t) {
    float r = std::sin(t * 2.0f) * 0.5f + 0.5f;
    float g = std::sin(t * 2.0f + 2.0f * 3.14159f / 3.0f) * 0.5f + 0.5f;
    float b = std::sin(t * 2.0f + 4.0f * 3.14159f / 3.0f) * 0.5f + 0.5f;
    return sf::Color(r * 255, g * 255, b * 255);
}

PlayerSprite::PlayerSprite(float x, float y) : healthBar(nullptr){
    if (!playerTexture.loadFromFile("img/sprite sheet-grayscale.png")) {
        std::cerr << "error loading sprite sheet" << std::endl;
    }
    playerSprite.setTexture(playerTexture);

    healthBar = new HealthBar(100.f, {10.f, 10.f});
    
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
    exploding = false; // Init

    velocity = sf::Vector2f(0.0f, 0.0f);
    health = 100.0f; 
    score = 0;

    particleSpawnTimer = 0.0f;
    totalTime = 0.0f;

    playerSprite.setPosition(position);
    playerSprite.setColor(color);
}

// NEW: Explode logic
void PlayerSprite::explode() {
    exploding = true;
    
    // Spawn 50 explosion particles
    for(int i = 0; i < 50; ++i) {
        Particle p;
        p.position = position;
        
        // Random angle and speed for burst
        float angle = (std::rand() % 360) * 3.14159f / 180.f;
        float speed = (std::rand() % 200 + 150); // Fast burst
        
        p.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        p.lifetime = sf::seconds(1.0f + (std::rand() % 10)/10.f); // 1-2 seconds life
        
        p.shape.setSize(sf::Vector2f(8, 8));
        p.shape.setOrigin(4, 4);
        p.shape.setPosition(p.position);
        p.color = color; // Explosion is player's color
        p.shape.setFillColor(p.color);
        
        particles.push_back(p);
    }
}

void PlayerSprite::update(float dt) {
    if (!isActive) return;

    // --- Particle Logic (Always run this, even if exploding) ---
    // Only spawn TRAIL particles if NOT exploding
    if (!exploding) {
        totalTime += dt;
        particleSpawnTimer += dt;
        if (particleSpawnTimer > 0.02f) {
            particleSpawnTimer = 0.0f;
            Particle p;
            p.position = position; 
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
    }

    // Update existing particles (for both trail and explosion)
    for (int i = particles.size() - 1; i >= 0; --i) {
        particles[i].lifetime -= sf::seconds(dt);
        if (particles[i].lifetime <= sf::Time::Zero) {
            particles.erase(particles.begin() + i);
        } else {
            particles[i].position += particles[i].velocity * dt;
            particles[i].shape.setPosition(particles[i].position);
            
            // fade out
            float maxLife = exploding ? 1.5f : 0.7f;
            float lifeRatio = particles[i].lifetime.asSeconds() / maxLife;
            sf::Color c = particles[i].color;
            c.a = static_cast<sf::Uint8>(lifeRatio * 255);
            particles[i].shape.setFillColor(c);
        }
    }

    // If exploding, STOP here. Do not process physics/animation.
    if (exploding) return;

    // --- Normal Physics ---
    velocity.y += fallStrength * dt;     
    position.y += velocity.y * dt;  
    playerSprite.setPosition(position); 

    // animation
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

    // Damage blinking
    if (damageCooldown > 0.f) {
        damageCooldown -= dt;
        blinkTimer -= dt;
        if (blinkTimer <= 0.f) {
            blinkTimer = blinkInterval;
            spriteVisible = !spriteVisible;
            playerSprite.setColor(spriteVisible ? color : sf::Color(color.r, color.g, color.b, 80));
        }
        if (damageCooldown <= 0.f) {
            spriteVisible = true;
            playerSprite.setColor(color);
            damageCooldown = 0.f;
        }
    }
}

void PlayerSprite::Draw(sf::RenderWindow& window) {
    if (!isActive) return;
    
    // draw particles first
    for (const auto& p : particles) {
        window.draw(p.shape);
    }
    
    // Draw player ONLY if not exploding
    if (!exploding) {
        window.draw(playerSprite);
        healthBar->Draw(window);
    }
}

void PlayerSprite::jump() {
    if (isActive && !exploding) velocity.y = jumpStrength; 
}

void PlayerSprite::changeColor(sf::Color newColor) {
    color = newColor;
    playerSprite.setColor(color);
}

void PlayerSprite::onCollision(const std::string& type, float damageAmount, Game* game) {
    if (type == "Obstacle") {
        if (damageCooldown <= 0.f) {
            health -= damageAmount;
            if (health < 0.f) health = 0.f;
            healthBar->damage(damageAmount);
            damageCooldown = damageDelay; 
            blinkTimer = blinkInterval;
        }
        velocity.y = 0.0f; 
    }
    else if (type == "Star") {
        score++;
        game->setScore();
    }
}

sf::Vector2f PlayerSprite::getPosition() const { return position; }
sf::FloatRect PlayerSprite::getBounds() const { return playerSprite.getGlobalBounds(); }
int PlayerSprite::getScore() const { return score; }
float PlayerSprite::getHealth() const { return health; }