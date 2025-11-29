#include "Header Files/Obstacle.hpp"
#include <cmath>

static const float PI = 3.14159265f;

ColorSquare::ColorSquare(float x, float y, float sideLength, float thickness, float rotationSpeed)
    : m_size(sideLength), m_thickness(thickness),
      m_rotationSpeed(rotationSpeed), m_rotation(0.f)
{
    // From Element
    position = sf::Vector2f(x, y);
    isActive = true;
    color = sf::Color::White; // base color, not used (each side has its own color)

    // From Obstacle
    size.clear();
    size.push_back(sideLength);
    size.push_back(thickness);

    float half = sideLength * 0.5f;
    float inner = half - thickness;

    // Define the 4 colors (same as wheel)
    m_colors[0] = sf::Color(255, 0, 255); // Pink  - Left
    // m_colors[0] = sf::Color::White;
    m_colors[1] = sf::Color::Blue;        // Blue  - Top
    m_colors[2] = sf::Color::Yellow;      // Yellow - Right
    m_colors[3] = sf::Color::Red;         // Red   - Bottom

    m_vertices.setPrimitiveType(sf::Quads);
m_vertices.resize(16); // 4 vertices × 4 sides

// float half = m_size * 0.5f;
// float inner = half - m_thickness;

// SIDE INDEXING
// 0–3   = Left
// 4–7   = Top
// 8–11  = Right
// 12–15 = Bottom

// Helper to write one quad
auto quad = [&](int start, sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d, sf::Color col)
{
    m_vertices[start + 0].position = a;
    m_vertices[start + 1].position = b;
    m_vertices[start + 2].position = c;
    m_vertices[start + 3].position = d;

    m_vertices[start + 0].color = col;
    m_vertices[start + 1].color = col;
    m_vertices[start + 2].color = col;
    m_vertices[start + 3].color = col;
};

// LEFT SIDE
quad(0,
     {-half, -half}, {-inner, -inner}, {-inner, inner}, {-half, half},
     m_colors[0]);

// TOP SIDE
quad(4,
     {-half, -half}, {half, -half}, {inner, -inner}, {-inner, -inner},
     m_colors[1]);

// RIGHT SIDE
quad(8,
     {half, -half}, {inner, -inner}, {inner, inner}, {half, half},
     m_colors[2]);

// BOTTOM SIDE
quad(12,
     {-inner, inner}, {inner, inner}, {half, half}, {-half, half},
     m_colors[3]);
}
//     // Square outline using triangle strips
//     m_vertices.setPrimitiveType(sf::TriangleStrip);
//     m_vertices.resize(16);

//     auto quad = [&](int idx, sf::Vector2f o1, sf::Vector2f o2, sf::Vector2f i1, sf::Vector2f i2, sf::Color c)
//     {
//         m_vertices[idx].position = o1;
//         m_vertices[idx].color = c;

//         m_vertices[idx + 1].position = i1;
//         m_vertices[idx + 1].color = c;

//         m_vertices[idx + 2].position = o2;
//         m_vertices[idx + 2].color = c;

//         m_vertices[idx + 3].position = i2;
//         m_vertices[idx + 3].color = c;
//     };

//     // Left
//     quad(0,
//         {-half, -half}, {-half, half},
//         {-inner, -inner}, {-inner, inner},
//         m_colors[0]);

//     // Top
//     quad(4,
//         {-half, -half}, {half, -half},
//         {-inner, -inner}, {inner, -inner},
//         m_colors[1]);

//     // Right
//     quad(8,
//         {half, -half}, {half, half},
//         {inner, -inner}, {inner, inner},
//         m_colors[2]);

//     // Bottom
//     quad(12,
//         {-half, half}, {half, half},
//         {-inner, inner}, {inner, inner},
//         m_colors[3]);
// }

void ColorSquare::update(float dt)
{
    if (!isActive) return;

    m_rotation += m_rotationSpeed * dt;
    if (m_rotation >= 360.f)
        m_rotation -= 360.f;
}

void ColorSquare::Draw(sf::RenderWindow& window)
{
    if (!isActive) return;

    sf::Transform transform;
    transform.translate(position);
    transform.rotate(m_rotation);

    sf::RenderStates states;
    states.transform = transform;

    window.draw(m_vertices, states);
}

sf::FloatRect ColorSquare::getBounds() const
{
    return sf::FloatRect(
        position.x - m_size * 0.5f,
        position.y - m_size * 0.5f,
        m_size,
        m_size
    );
}

bool ColorSquare::checkCollision(sf::FloatRect, sf::Color playerColor, sf::Vector2f playerPos) const
{
    float half = m_size * 0.5f;
    float inner = half - m_thickness;

    // Convert player position → local square coordinates
    sf::Transform inv;
    inv.translate(position);
    inv.rotate(m_rotation);
    inv = inv.getInverse();

    sf::Vector2f local = inv.transformPoint(playerPos);

    float x = local.x;
    float y = local.y;

    // Out of square range entirely → no collision
    if (std::abs(x) > half + 2 || std::abs(y) > half + 2)
        return false;

    int touchedSide = -1;

    // Detect which side player is touching
    if (std::abs(x) >= inner && std::abs(x) <= half)
        touchedSide = (x < 0 ? 0 : 2); // Left or Right
    else if (std::abs(y) >= inner && std::abs(y) <= half)
        touchedSide = (y < 0 ? 1 : 3); // Top or Bottom

    if (touchedSide == -1)
        return false;

    sf::Color sideColor = getColorAtSide(touchedSide);

    return sideColor != playerColor;
}

sf::Color ColorSquare::getColorAtSide(int side) const
{
    return m_colors[side];
}
