#pragma once
#include <SFML/Graphics.hpp>
#include "Element.hpp"   // include the base class

class HealthBar : public Element {
public:
    HealthBar(float maxHealth, sf::Vector2f pos, sf::Vector2f size)
        : m_maxHealth(maxHealth), m_currentHealth(maxHealth)
    {
        position = pos;        // from Element
        color = sf::Color::White;
        isActive = true;

        m_size = size;

        // Background
        m_background.setSize(size);
        m_background.setFillColor(sf::Color(110, 80, 150));   // lavender
        m_background.setPosition(position);

        // Foreground (actual health)
        m_foreground.setSize(size);
        m_foreground.setFillColor(sf::Color(219, 185, 255));  // lilac
        m_foreground.setPosition(position);
    }

    void update(float dt) override {
        // Nothing needed for now
    }

    void Draw(sf::RenderWindow& window) override {
        window.draw(m_background);
        window.draw(m_foreground);
    }

    

    void setHealth(float hp) {
        m_currentHealth = std::max(0.f, std::min(hp, m_maxHealth));

        float percent = m_currentHealth / m_maxHealth;

        m_foreground.setSize({ m_size.x * percent, m_size.y });
    }

    void damage(float amount) {
        setHealth(m_currentHealth - amount);
    }

    void heal(float amount) {
        setHealth(m_currentHealth + amount);
    }

private:
    float m_maxHealth;
    float m_currentHealth;

    sf::Vector2f m_size;

    sf::RectangleShape m_background;
    sf::RectangleShape m_foreground;
};

