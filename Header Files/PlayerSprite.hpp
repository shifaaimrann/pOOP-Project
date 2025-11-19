#pragma once
#include "Element.hpp"
#include <string> 
#include <vector>
#include <SFML/Graphics.hpp>

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
    virtual ~PlayerSprite() {} 

    virtual void update(float dt) override;
    virtual void Draw(sf::RenderWindow& window) override;

    void jump();
    void changeColor(sf::Color newColor);
    void onCollision(const std::string& type); 

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
};