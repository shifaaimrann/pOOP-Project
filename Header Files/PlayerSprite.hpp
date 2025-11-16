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

    sf::Vector2f getPosition() const; // <-- re-added for particle trail

private:
    sf::Texture playerTexture;
    sf::Sprite  playerSprite; 

    sf::Vector2f velocity;
    int          health;
    
    sf::IntRect currentFrameRect;
    int   frameWidth;
    int   frameHeight;
    int   frameCount;
    float animationTimer;
};