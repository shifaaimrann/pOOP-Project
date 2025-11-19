#pragma once
#include <SFML/Graphics.hpp>

// an abstract base class for all game objects(like the player, stars, or obstacles)
class Element {
public:
    virtual ~Element() {} 
    virtual void update(float ) = 0; 
    virtual void Draw(sf::RenderWindow& window) = 0;

    
    void setPosition(float x, float y) { 
        position = sf::Vector2f(x, y); 
    }
    
    // needed for wheel movement
    void move(float x, float y) { 
        position.x += x; 
        position.y += y; 
    }
    
    sf::Vector2f getPosition() const { 
        return position; 
    }
    
    // needed for color collision
    sf::Color getColor() const { 
        return color; 
    }

protected: 
    sf::Vector2f position;
    sf::Color    color; 
    bool         isActive;
};