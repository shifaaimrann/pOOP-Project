#pragma once
#include "Element.hpp"

class PlayerSprite : public Element {
public:
    PlayerSprite(float x, float y);//starting position
    virtual ~PlayerSprite() {}
    virtual void update(float c) override;
    virtual void Draw(sf::RenderWindow& window) override;

    void jump();                          
    void changeColor(sf::Color newColor);  
    void onCollision();                  

private:
    sf::CircleShape shape;//to be replaced w sprite sheet tmr hopefully
    sf::Vector2f velocity; // 
    float        radius;   // 
    int          health;   // 
};