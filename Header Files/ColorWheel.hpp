#pragma once
#include "Obstacle.hpp" 
#include <cmath>       

class ColorWheel : public Obstacle {
public:
    ColorWheel(float x, float y, float radius, float thickness, float speed);//position size n speed are in constructor
    virtual ~ColorWheel() {}
    virtual void update(float dt) override;
    virtual void Draw(sf::RenderWindow& window) override;
    virtual sf::FloatRect getBounds() const override;

    // --- colorwheel-specific functions ---
    sf::Color getColorAtAngle(float worldAngle) const;
    sf::Color getPlayerColor() const; // get the player's current color

private:
    sf::VertexArray m_vertices; // for drawing the wheel
    float m_rotationSpeed;
    float m_rotation; // current rotation in degrees
    float m_radius;   // store the radius for bounds checking
    
    // a private constant for math
    const float PI = 3.14159265f;

    // array to hold the wheel's colors in order
    sf::Color m_colors[4];
};