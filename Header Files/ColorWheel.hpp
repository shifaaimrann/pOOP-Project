#pragma once
#include "Obstacle.hpp" 
#include <cmath>       

class ColorWheel : public Obstacle {
public:
    ColorWheel(float x, float y, float radius, float thickness, float speed);
    virtual ~ColorWheel() {}
    
    virtual void update(float dt) override;
    virtual void Draw(sf::RenderWindow& window) override;
    virtual sf::FloatRect getBounds() const override;

    // --- NEW: The complex math goes here, hiding it from Main ---
    virtual bool checkCollision(sf::FloatRect playerBounds, sf::Color playerColor, sf::Vector2f playerPos) const override;

    float getDamage() const override {return 8.0f;}
    sf::Color getColorAtAngle(float worldAngle) const;

private:
    sf::VertexArray m_vertices; 
    float m_rotationSpeed;
    float m_rotation; 
    float m_radius;   
    float m_thickness; // Added to store thickness for collision math
    
    const float PI = 3.14159265f;
    sf::Color m_colors[4];
};