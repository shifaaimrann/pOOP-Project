#pragma once
#include "Element.hpp"
#include <string> 
#include <vector>
#include <map> // ADD THIS
#include <SFML/Graphics.hpp>
#include "HealthBar.hpp"

class Game;

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Time     lifetime;
    sf::RectangleShape shape;
    sf::Color    color;
};

class PlayerSprite : public Element {
public:
    PlayerSprite(float x, float y); 
    virtual ~PlayerSprite() { delete healthBar; } 

    virtual void update(float dt) override;
    virtual void Draw(sf::RenderWindow& window) override;

    void jump();
    
    // CHANGED: Now switches texture based on predefined colors
    void setRandomColor(); 
    
    void onCollision(const std::string& type, float, Game* game); 
    inline void setHealth(float hp) {
        health = std::max(0.f, std::min(hp, 100.f));
        healthBar->setHealth(hp);
    }

    void explode();

    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const; 
    int getScore() const;
    float getHealth() const;

private:
    sf::Sprite  playerSprite; 
    
    // NEW: Texture storage
    std::map<std::string, sf::Texture> textures;
    std::string currentColorKey;

    sf::Vector2f velocity;
    float        health; 
    int          score;  
    HealthBar* healthBar;
    
    sf::IntRect currentFrameRect; 
    int   frameWidth;
    int   frameHeight;
    int   frameCount;
    float animationTimer;

    std::vector<Particle> particles;
    float particleSpawnTimer;
    float totalTime; 

    float damageCooldown = 0.f;
    const float damageDelay = 0.4f; 
    bool spriteVisible = true;
    float blinkTimer = 0.f;
    const float blinkInterval = 0.07f;

    bool exploding = false;
    
    // Helper to swap texture
    void applyTexture(const std::string& colorKey);
};