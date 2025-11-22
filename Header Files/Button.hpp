// Header Files/Button.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

class Button {
public:
    // pass font by reference to save memory
    Button(float x, float y, float w, float h, std::string text, sf::Font& font);

    // swap the gray box for a real image
    void setTexture(const std::string& filename);

    void draw(sf::RenderWindow& window);
    
    // checks if mouse clicked inside this button
    bool isClicked(sf::Event& event, const sf::RenderWindow& window);

private:
    sf::RectangleShape shape; // placeholder graphic
    sf::Sprite sprite;        // image graphic
    sf::Texture texture;      // holds image data
    bool hasTexture;          // switch between shape and sprite

    sf::Text buttonText;      
};