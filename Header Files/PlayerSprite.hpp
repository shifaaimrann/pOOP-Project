#pragma once
#include "Element.hpp"
#include <string> 
#include <vector>
#include <SFML/Graphics.hpp>
#include "HealthBar.hpp"
#include "game.hpp"

class Game;
// simple struct for the trail
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
    virtual ~PlayerSprite() {delete healthBar;} 

    virtual void update(float dt) override;
    virtual void Draw(sf::RenderWindow& window) override;

    void jump();
    void changeColor(sf::Color newColor);
    void onCollision(const std::string& type, float, Game* game); 
    inline void setHealth(float hp) {
        health = std::max(0.f, std::min(hp, 100.f));
        healthBar->setHealth(hp);
    }


    // getters
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const; 
    int getScore() const;
    float getHealth() const;

private:
    sf::Texture playerTexture; 
    sf::Sprite  playerSprite; 
    
    sf::Vector2f velocity;
    float        health; 
    int          score;  
    HealthBar* healthBar;
    
    // animation vars
    sf::IntRect currentFrameRect; 
    int   frameWidth;
    int   frameHeight;
    int   frameCount;
    float animationTimer;

    // particles vars
    std::vector<Particle> particles;
    float particleSpawnTimer;
    float totalTime; // for rainbow effect

    float damageCooldown = 0.f;
    const float damageDelay = 0.4f;  // 0.4 seconds between hits
    bool spriteVisible = true;
    float blinkTimer = 0.f;
    const float blinkInterval = 0.07f;




};