#pragma once
#include "Element.hpp"

class PlayerSprite : public Element {
public:
    PlayerSprite(float x, float y);
    virtual ~PlayerSprite() {}
    virtual void update(float dt) override;
    virtual void Draw(sf::RenderWindow& window) override;
    void jump();
    void changeColor(sf::Color newColor);
    void onCollision();
    

private:
    sf::Texture playerTexture;//grayscale spritesheet
    sf::Sprite  playerSprite;  
    sf::Vector2f velocity;
    int          health;
    sf::IntRect currentFrameRect;//whihc frame we ae at (out of 4)
    int   frameWidth;            
    int   frameHeight;           
    int   frameCount;        
    float animationTimer; 
};