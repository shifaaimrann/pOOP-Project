#include "Header Files/PlayerSprite.hpp"
#include "Header Files/game.hpp"
#include <iostream>
#include <cmath> 

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
  
    if (!textures["Pink"].loadFromFile("img/playersprites/player_pink.png")) std::cerr << "Missing player_pink.png\n";
    if (!textures["Blue"].loadFromFile("img/playersprites/player_blue.png")) std::cerr << "Missing player_blue.png\n";
    if (!textures["Yellow"].loadFromFile("img/playersprites/player_yellow.png")) std::cerr << "Missing player_yellow.png\n";
    if (!textures["Red"].loadFromFile("img/playersprites/player_red.png")) std::cerr << "Missing player_red.png\n";

    healthBar = new HealthBar(100.f, {10.f, 10.f});
    
    frameWidth = 256;  
    frameHeight = 256;
    frameCount = 4;
    animationTimer = 0.0f;

    currentFrameRect = sf::IntRect(0, 0, frameWidth, frameHeight);
    
    playerSprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    playerSprite.setScale(0.32f, 0.32f); 

    position = sf::Vector2f(x, y);
    
    // Set initial color 
    applyTexture("Pink");
    
    isActive = true;
    exploding = false; 
    velocity = sf::Vector2f(0.0f, 0.0f);
    health = 100.0f; 
    score = 0;
    particleSpawnTimer = 0.0f;
    totalTime = 0.0f;

    playerSprite.setPosition(position);
}

// Switch to a specific texture map
void PlayerSprite::applyTexture(const std::string& colorKey) {
    currentColorKey = colorKey;
    playerSprite.setTexture(textures[colorKey]);
    playerSprite.setTextureRect(currentFrameRect); // Re-apply rect to new texture

    if (colorKey == "Pink") color = sf::Color(255, 0, 255); // Magenta
    else if (colorKey == "Blue") color = sf::Color::Blue;
    else if (colorKey == "Yellow") color = sf::Color::Yellow;
    else if (colorKey == "Red") color = sf::Color::Red;
    

    playerSprite.setColor(sf::Color::White); // Reset tint to pure
}

void PlayerSprite::setRandomColor() {
    // Define available colors
    std::vector<std::string> colors = {"Pink", "Blue", "Yellow", "Red"};
    std::string newColor;

    // Keep picking a random color until it is different from the current one
    do {
        int r = std::rand() % 4;
        newColor = colors[r];
    } while (newColor == currentColorKey);

    applyTexture(newColor);
}

void PlayerSprite::explode() {
    exploding = true;
    for(int i = 0; i < 50; ++i) {
        Particle p;
        p.position = position;
        float angle = (std::rand() % 360) * 3.14159f / 180.f;
        float speed = (std::rand() % 200 + 150); 
        p.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        p.lifetime = sf::seconds(1.0f + (std::rand() % 10)/10.f); 
        p.shape.setSize(sf::Vector2f(8, 8));
        p.shape.setOrigin(4, 4);
        p.shape.setPosition(p.position);
        p.color = color; 
        p.shape.setFillColor(p.color);
        particles.push_back(p);
    }
}

void PlayerSprite::update(float dt) {
    if (!isActive) return;

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

    for (int i = particles.size() - 1; i >= 0; --i) {
        particles[i].lifetime -= sf::seconds(dt);
        if (particles[i].lifetime <= sf::Time::Zero) {
            particles.erase(particles.begin() + i);
        } else {
            particles[i].position += particles[i].velocity * dt;
            particles[i].shape.setPosition(particles[i].position);
            float maxLife = exploding ? 1.5f : 0.7f;
            float lifeRatio = particles[i].lifetime.asSeconds() / maxLife;
            sf::Color c = particles[i].color;
            c.a = static_cast<sf::Uint8>(lifeRatio * 255);
            particles[i].shape.setFillColor(c);
        }
    }

    if (exploding) return;

    velocity.y += fallStrength * dt;     
    position.y += velocity.y * dt;  
    playerSprite.setPosition(position); 

    // Animation Logic
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

    // Blinking (Opacity)
    if (damageCooldown > 0.f) {
        damageCooldown -= dt;
        blinkTimer -= dt;
        if (blinkTimer <= 0.f) {
            blinkTimer = blinkInterval;
            spriteVisible = !spriteVisible;
            // Modify alpha of white (no tint)
            sf::Color c = sf::Color::White;
            c.a = spriteVisible ? 255 : 80;
            playerSprite.setColor(c);
        }
        if (damageCooldown <= 0.f) {
            spriteVisible = true;
            playerSprite.setColor(sf::Color::White);
            damageCooldown = 0.f;
        }
    }
}

void PlayerSprite::Draw(sf::RenderWindow& window) {
    if (!isActive) return;
    for (const auto& p : particles) window.draw(p.shape);
    if (!exploding) {
        window.draw(playerSprite);
        healthBar->Draw(window);
    }
}

void PlayerSprite::jump() {
    if (isActive && !exploding) velocity.y = jumpStrength; 
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
        if(game) game->setScore();
    }
    // Color Changer Logic
    else if (type == "ColorChanger") {
        setRandomColor();
        // Optional: Small upward boost when changing color?
        // velocity.y = jumpStrength / 2; 
    }
}

// Getters
sf::Vector2f PlayerSprite::getPosition() const { return position; }
sf::FloatRect PlayerSprite::getBounds() const { return playerSprite.getGlobalBounds(); }
int PlayerSprite::getScore() const { return score; }
float PlayerSprite::getHealth() const { return health; }