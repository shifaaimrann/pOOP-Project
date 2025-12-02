#include "Header Files/Obstacle.hpp"
#include <cmath>

static constexpr float PI = 3.14159265f;

ColorSpotWheel::ColorSpotWheel(float x, float y, float radius, float spotRadius, int spotCount, float rotationSpeed)
    : m_radius(radius),
      m_spotRadius(spotRadius),
      m_spotCount(spotCount),
      m_rotationSpeed(rotationSpeed),
      m_rotation(0.f)
{
    position = {x, y};
    isActive = true;

    size.clear();
    size.push_back(radius);
    size.push_back(spotRadius);

    // Colors same as your wheel
    m_colors[0] = sf::Color(255, 0, 255); // Pink
    m_colors[1] = sf::Color::Blue;
    m_colors[2] = sf::Color::Yellow;
    m_colors[3] = sf::Color::Red;

    // One circle reused to draw all spots
    m_spot.setRadius(m_spotRadius);
    m_spot.setOrigin(m_spotRadius, m_spotRadius);
    m_spot.setPointCount(32);

    // Pre-assign colors to each spot
    m_spotColors.resize(m_spotCount);
    for (int i = 0; i < m_spotCount; i++)
    {
        // Determine which 90° sector this spot is in
        int sector = (i * 360 / m_spotCount) / 90;
        m_spotColors[i] = m_colors[sector % 4];
}

}

void ColorSpotWheel::update(float dt)
{
    if (!isActive) return;

    m_rotation += m_rotationSpeed * dt;
    if (m_rotation >= 360.f)
        m_rotation -= 360.f;
}

void ColorSpotWheel::Draw(sf::RenderWindow& window)
{
    if (!isActive) return;

    for (int i = 0; i < m_spotCount; i++)
    {
        float angle = (360.0f / m_spotCount) * i + m_rotation;
        float rad = angle * PI / 180.0f;

        float x = position.x + m_radius * std::cos(rad);
        float y = position.y + m_radius * std::sin(rad);

        // Color selection based on the same 90° sector rules
        // m_spot.setFillColor(getColorAtAngle(angle));
        m_spot.setFillColor(m_spotColors[i]);


        m_spot.setPosition(x, y);
        window.draw(m_spot);
    }
}

sf::FloatRect ColorSpotWheel::getBounds() const
{
    return sf::FloatRect(
        position.x - m_radius - m_spotRadius,
        position.y - m_radius - m_spotRadius,
        (m_radius + m_spotRadius) * 2,
        (m_radius + m_spotRadius) * 2
    );
}


bool ColorSpotWheel::checkCollision(sf::FloatRect, sf::Color playerColor, sf::Vector2f playerPos) const
{
    float dx = playerPos.x - position.x;
    float dy = playerPos.y - position.y;
    float dist = std::sqrt(dx*dx + dy*dy);

    // Spot ring collision range
    if (dist < m_radius - m_spotRadius * 0.5f ||
        dist > m_radius + m_spotRadius * 0.5f)
        return false;

    // Angle of player
    float angle = std::atan2(dy, dx) * 180.f / PI;
    if (angle < 0) angle += 360.f;

    sf::Color requiredColor = getColorAtAngle(angle);

    return playerColor != requiredColor;
}

sf::Color ColorSpotWheel::getColorAtAngle(float worldAngle) const
{
    float local = fmod(worldAngle - m_rotation, 360.f);
    if (local < 0) local += 360.f;

    int colorIndex = static_cast<int>(local / 90.f) % 4;
    return m_colors[colorIndex];
}
