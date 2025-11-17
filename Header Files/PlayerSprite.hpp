#pragma once
#include "Element.hpp" // inherits from our base class

class PlayerSprite : public Element {
public:
    PlayerSprite(float x, float y); // constructor
    virtual ~PlayerSprite() {} // virtual destructor

    // implementing the pure virtual functions from Element
    virtual void update(float dt) override;
    virtual void Draw(sf::RenderWindow& window) override;

    // player-specific actions
    void jump();
    void changeColor(sf::Color newColor);
    void onCollision(); // for later use

    // "getter" functions so main.cpp can see player info
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const; // for collision detection

private:
    sf::Texture playerTexture; // the 1024x256 grayscale sprite sheet
    sf::Sprite  playerSprite;  // the object we actually draw and move

    // physics
    sf::Vector2f velocity;
    int          health;
    
    // animation
    sf::IntRect currentFrameRect; // the 256x256 part of the sheet we're drawing
    int   frameWidth;
    int   frameHeight;
    int   frameCount;
    float animationTimer;
};