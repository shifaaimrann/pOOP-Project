#pragma once
#include <SFML/Graphics.hpp>
#include "Element.hpp"

class HealthBar : public Element {
public:
    HealthBar(float maxHealth, sf::Vector2f pos): m_maxHealth(maxHealth), m_currentHealth(maxHealth){
        position = pos;
        isActive = true;
        emptyTex.loadFromFile("img/health_zero1.png");         // Load textures
        fullTex.loadFromFile("img/health_full.png");
        emptySprite.setTexture(emptyTex);
        fullSprite.setTexture(fullTex);
        emptySprite.setPosition(position);
        fullSprite.setPosition(position);

        // Store full width for cropping
        fullWidth = fullTex.getSize().x;

        // Ensure fullSprite initially uses the whole texture
        fullSprite.setTextureRect(sf::IntRect(0, 0, fullWidth, fullTex.getSize().y));
        emptySprite.setScale(0.09f, 0.09f);  // adjust scale as needed
        fullSprite.setScale(0.09f, 0.09f);

    }

    void update(float dt) override {}

    void Draw(sf::RenderWindow& window) override {
        window.draw(emptySprite);   // Background bar
        window.draw(fullSprite);    // Fill bar (cropped)
    }
    float getCurrentHealth() const { 
    return m_currentHealth; 
}

    void setHealth(float hp) {
        m_currentHealth = std::max(0.f, std::min(hp, m_maxHealth));
        float percent = m_currentHealth / m_maxHealth;

        // Crop full bar horizontally
        sf::IntRect rect = fullSprite.getTextureRect();
        rect.width = static_cast<int>(fullWidth * percent);
        fullSprite.setTextureRect(rect);
    }

    void damage(float amount) {setHealth(m_currentHealth - amount);}
    void heal(float amount)   { setHealth(m_currentHealth + amount); }


private:
    float m_maxHealth;
    float m_currentHealth;

    sf::Texture emptyTex;  // 0% bar
    sf::Texture fullTex;   // 100% bar

    sf::Sprite emptySprite;
    sf::Sprite fullSprite;

    int fullWidth;         // original width of full bar
};


