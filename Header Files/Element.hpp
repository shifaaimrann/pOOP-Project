#pragma once
#include <SFML/Graphics.hpp>

// an abstract base class for all game objects
// (like the player, stars, or obstacles)
// it defines the "rules" that all child classes must follow
class Element {
public:
    virtual ~Element() {} // virtual destructor for base class

    // pure virtual update function, must be implemented by children
    // 'dt' is delta time, ensuring smooth, frame-independent movement
    virtual void update(float dt) = 0; 
    
    // pure virtual draw function
    virtual void Draw(sf::RenderWindow& window) = 0;

protected: // 'protected' lets child classes (like PlayerSprite) use these
    sf::Vector2f position;
    sf::Color    color; 
    bool         isActive;
};