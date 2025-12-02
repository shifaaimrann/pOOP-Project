// Button.cpp
#include "Header Files/Button.hpp"

Button::Button(float x, float y, float w, float h, std::string text, sf::Font& font) {
    hasTexture = false;

    // setup default gray placeholder
    shape.setPosition(x, y);
    shape.setSize(sf::Vector2f(w, h));
    shape.setFillColor(sf::Color(100, 100, 100)); 
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::White);

    // setup text label
    buttonText.setFont(font);
    buttonText.setString(text);
    buttonText.setCharacterSize(24);
    buttonText.setFillColor(sf::Color::White);
    
    // center the text using math
    sf::FloatRect tr = buttonText.getLocalBounds();
    buttonText.setOrigin(tr.left + tr.width / 2.0f, tr.top + tr.height / 2.0f);
    buttonText.setPosition(x + w / 2.0f, y + h / 2.0f);
}

void Button::setTexture(const std::string& filename) {
    // try to load the image
    if (texture.loadFromFile(filename)) {
        sprite.setTexture(texture);
        sprite.setPosition(shape.getPosition());
        
      
        sf::Vector2u tSize = texture.getSize();
        sf::Vector2f bSize = shape.getSize();

        float scaleX = bSize.x / tSize.x;
        float scaleY = bSize.y / tSize.y;

        sprite.setScale(scaleX, scaleY);
        

        hasTexture = true;
    }
}

void Button::draw(sf::RenderWindow& window) {
    // draw sprite if loaded otherwise draw shape
    if (hasTexture) window.draw(sprite);
    else window.draw(shape);
    
    window.draw(buttonText);
}

bool Button::isClicked(sf::Event& event, const sf::RenderWindow& window) {
    // check if left mouse button was released
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        // check collision with whatever visual is active
        if (hasTexture) return sprite.getGlobalBounds().contains(worldPos);
        else return shape.getGlobalBounds().contains(worldPos);
    }
    return false;
}