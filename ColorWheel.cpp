#include "Header Files/ColorWheel.hpp"
#include <cmath>
#include <iostream>

ColorWheel::ColorWheel(float x, float y, float radius, float thickness, float speed) 
    : m_rotationSpeed(speed), m_rotation(0.f), m_radius(radius), m_thickness(thickness) {

    position = sf::Vector2f(x, y);
    isActive = true;

    const int points = 100;
    float innerRadius = radius - thickness;

    m_vertices.setPrimitiveType(sf::TriangleStrip);
    m_vertices.resize((points + 1) * 2);

    m_colors[0] = sf::Color(255, 0, 255); // Pink
    m_colors[1] = sf::Color::Blue;
    m_colors[2] = sf::Color::Yellow;
    m_colors[3] = sf::Color::Red;

    for (int i = 0; i <= points; ++i) {
        float angle = (static_cast<float>(i) / points) * 360.f;
        float rad = angle * (PI / 180.f);

        sf::Vector2f outerPoint(radius * std::cos(rad), radius * std::sin(rad));
        sf::Vector2f innerPoint(innerRadius * std::cos(rad), innerRadius * std::sin(rad));

        int colorIndex = static_cast<int>(angle / 90.f) % 4;

        m_vertices[i * 2].position = outerPoint;
        m_vertices[i * 2].color = m_colors[colorIndex];
        m_vertices[i * 2 + 1].position = innerPoint;
        m_vertices[i * 2 + 1].color = m_colors[colorIndex];
    }
}

void ColorWheel::update(float dt) {
    if (!isActive) return;
    m_rotation += m_rotationSpeed * dt;
    if (m_rotation > 360.f) m_rotation -= 360.f;
}

void ColorWheel::Draw(sf::RenderWindow& window) {
    if (!isActive) return;
    sf::Transform transform;
    transform.translate(position); 
    transform.rotate(m_rotation);  
    sf::RenderStates states;
    states.transform = transform;
    window.draw(m_vertices, states);
}

sf::FloatRect ColorWheel::getBounds() const {
    return sf::FloatRect(position.x - m_radius, position.y - m_radius, m_radius * 2, m_radius * 2);
}

// --- THE FIXED COLLISION LOGIC ---
bool ColorWheel::checkCollision(sf::FloatRect playerBounds, sf::Color playerColor, sf::Vector2f playerPos) const {
    float dx = playerPos.x - position.x;
    float dy = playerPos.y - position.y;
    float dist = std::sqrt(dx*dx + dy*dy);

    float innerRad = m_radius - m_thickness;

    // Check if player is inside the ring (between inner and outer radius)
    // We give a tiny buffer (5.0f) so it's not too punishing
    if (dist <= m_radius && dist >= innerRad - 5.0f) {
        
        // Calculate angle in degrees (0 to 360)
        float angle = std::atan2(dy, dx) * 180.f / PI;
        if (angle < 0) angle += 360.f;

        // Get the color of the wheel at this specific angle
        sf::Color segmentColor = getColorAtAngle(angle);

        // If colors DON'T match, it is a collision
        if (segmentColor != playerColor) {
            return true;
        }
    }
    return false;
}

sf::Color ColorWheel::getColorAtAngle(float worldAngle) const {
    // Adjust world angle by current rotation
    float localAngle = fmod(worldAngle - m_rotation, 360.f);
    if (localAngle < 0) localAngle += 360.f;

    int colorIndex = static_cast<int>(localAngle / 90.f) % 4;
    return m_colors[colorIndex];
}