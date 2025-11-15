#pragma once
#include <SFML/Graphics.hpp>

//includes player stars, colorwheel etc
class Element {
public:
    virtual ~Element() {}
    virtual void update(float c) = 0; 
    virtual void Draw(sf::RenderWindow& window) = 0;
protected://so child classes can access thees
    sf::Vector2f position;
    sf::Color    color; 
    bool         isActive;
};