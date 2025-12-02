#include "Header Files/Obstacle.hpp"
#include <cmath>

ColorCross::ColorCross(float x, float y, float armLength, float thickness, float rotationSpeed)
    : m_armLength(armLength), m_thickness(thickness), m_rotationSpeed(rotationSpeed), m_rotation(0.f)
{
    position = {x, y};
    isActive = true;

    size.clear();
    size.push_back(armLength);
    size.push_back(thickness);

    // Colors: vertical top/bottom, horizontal left/right
    m_colors[0] = sf::Color(255,0,255); // Pink (Top)
    m_colors[1] = sf::Color::Blue;      // Blue (Left)
    m_colors[2] = sf::Color::Yellow;    // Yellow (Right)
    m_colors[3] = sf::Color::Red;       // Red (Bottom)

    // Vertical arm
    m_arm1.setSize({m_thickness, m_armLength});
    m_arm1.setOrigin(m_thickness/2.f, m_armLength/2.f);

    // Horizontal arm
    m_arm2.setSize({m_armLength, m_thickness});
    m_arm2.setOrigin(m_armLength/2.f, m_thickness/2.f);
}

void ColorCross::update(float dt)
{
    if (!isActive) return;
    m_rotation += m_rotationSpeed * dt;
    if (m_rotation >= 360.f) m_rotation -= 360.f;
}

void ColorCross::Draw(sf::RenderWindow& window)
{
    if (!isActive) return;

    sf::Transform transform;
    transform.translate(position);
    transform.rotate(m_rotation);

    // Vertical arm: top = Pink, bottom = Red
    sf::Transform t = transform;
    // Split into top/bottom halves as two rectangles for color
    sf::RectangleShape topRect({m_thickness, m_armLength/2.f});
    topRect.setOrigin(m_thickness/2.f, m_armLength/4.f);
    topRect.setPosition(0, -m_armLength/4.f);
    topRect.setFillColor(m_colors[0]);
    window.draw(topRect, t);

    sf::RectangleShape bottomRect({m_thickness, m_armLength/2.f});
    bottomRect.setOrigin(m_thickness/2.f, m_armLength/4.f);
    bottomRect.setPosition(0, m_armLength/4.f);
    bottomRect.setFillColor(m_colors[3]);
    window.draw(bottomRect, t);

    // Horizontal arm: left = Blue, right = Yellow
    sf::RectangleShape leftRect({m_armLength/2.f, m_thickness});
    leftRect.setOrigin(m_armLength/4.f, m_thickness/2.f);
    leftRect.setPosition(-m_armLength/4.f, 0);
    leftRect.setFillColor(m_colors[1]);
    window.draw(leftRect, t);

    sf::RectangleShape rightRect({m_armLength/2.f, m_thickness});
    rightRect.setOrigin(m_armLength/4.f, m_thickness/2.f);
    rightRect.setPosition(m_armLength/4.f, 0);
    rightRect.setFillColor(m_colors[2]);
    window.draw(rightRect, t);
}

sf::FloatRect ColorCross::getBounds() const
{
    return sf::FloatRect(position.x - m_armLength/2.f, position.y - m_armLength/2.f, m_armLength, m_armLength);
}


bool ColorCross::checkCollision(sf::FloatRect, sf::Color playerColor, sf::Vector2f playerPos) const
{
    // Transform player position into cross's local coordinates
    sf::Transform inv;
    inv.translate(position);
    inv.rotate(m_rotation);
    inv = inv.getInverse();

    sf::Vector2f local = inv.transformPoint(playerPos);

    // Vertical arm
    if (std::abs(local.x) <= m_thickness/2.f && std::abs(local.y) <= m_armLength/2.f)
    {
        sf::Color col = (local.y < 0 ? m_colors[0] : m_colors[3]);
        return col != playerColor;
    }

    // Horizontal arm
    if (std::abs(local.y) <= m_thickness/2.f && std::abs(local.x) <= m_armLength/2.f)
    {
        sf::Color col = (local.x < 0 ? m_colors[1] : m_colors[2]);
        return col != playerColor;
    }

    return false; // outside cross
}

