#include <SFML/Graphics.hpp>
#include <iostream>
// #include "Header Files/PlayerSprite.hpp"
// #include "Header Files/HealthBar.hpp"
// #include "Header Files/Obstacle.hpp"
// #include "Header Files/ColorWheel.hpp"
// #include "Header Files/star.hpp"
// #include "Header Files/Level.hpp"

#include <SFML/Graphics.hpp>
#include "Header Files/Level.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Miffy Switch");
    window.setFramerateLimit(60);
    Level level(window);
    level.loadLevel(1); 
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            
            level.handleInput(event);
        }

        // Calculate time passed
        float dt = clock.restart().asSeconds();
        
        // Clear the screen (using a dark color for background)
        window.clear(sf::Color(0, 0, 0)); 
        
        // Update and Draw the level
        level.update(dt);
        level.draw();

        window.display();
    }

    return 0;
}