#include "Header Files/ColorWheel.hpp"
#include <cmath> // for M_PI, sin, cos, fmod

// constructor: builds the wheel geometry
ColorWheel::ColorWheel(float x, float y, float radius, float thickness, float speed) 
    : m_rotationSpeed(speed), m_rotation(0.f), m_radius(radius) {

    // set properties from the element base class
    position = sf::Vector2f(x, y);
    isActive = true;

    const int points = 100; // how many points to use for a full circle
    float innerRadius = radius - thickness;

    m_vertices.setPrimitiveType(sf::TriangleStrip);
    m_vertices.resize((points + 1) * 2); // (points + 1) * 2 vertices

    // store our colors in the member array
    m_colors[0] = sf::Color(255, 0, 255); // 0: pink
    m_colors[1] = sf::Color::Blue;        // 1: blue
    m_colors[2] = sf::Color::Yellow;      // 2: yellow
    m_colors[3] = sf::Color::Red;         // 3: red

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

// update method to spin the wheel
void ColorWheel::update(float dt) {
    if (!isActive) return;
    
    // update the rotation based on speed and delta time
    m_rotation += m_rotationSpeed * dt;
    if (m_rotation > 360.f) {
        m_rotation -= 360.f;
    } else if (m_rotation < 0.f) {
        m_rotation += 360.f;
    }
}

// this is the draw function we must implement
void ColorWheel::Draw(sf::RenderWindow& window) {
    if (!isActive) return;

    // create a transform to position and rotate the wheel
    sf::Transform transform;
    transform.translate(position); // 1. move to its position
    transform.rotate(m_rotation);  // 2. rotate around its local origin (0,0)

    // draw the vertices using our custom transform
    sf::RenderStates states;
    states.transform = transform;
    window.draw(m_vertices, states);
}

// returns a collision box based on the wheel's radius
sf::FloatRect ColorWheel::getBounds() const {
    // return a square box that contains the whole circle
    return sf::FloatRect(position.x - m_radius, position.y - m_radius, m_radius * 2, m_radius * 2);
}

// gets the color at a specific angle (e.g., 270 degrees for the bottom)
sf::Color ColorWheel::getColorAtAngle(float worldAngle) const {
    // adjust the world angle by the wheel's current rotation
    float localAngle = fmod(worldAngle - m_rotation, 360.f);
    if (localAngle < 0) {
        localAngle += 360.f;
    }

    // find the quadrant
    int colorIndex = static_cast<int>(localAngle / 90.f) % 4;
    return m_colors[colorIndex];
}