#include <SFML/Graphics.hpp>
#include "PlayerSprite.hpp" // Include the player class you just made

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Color Switch");
    window.setFramerateLimit(60);
    PlayerSprite player(400.0f, 300.0f);
    sf::Clock clock;

    sf::Color colors[] = { 
        sf::Color::White, 
        sf::Color::Blue, 
        sf::Color::Yellow, 
        sf::Color(255, 0, 255) // Pink
    };
    int colorIndex = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Space) {
                    player.jump();
                }
                if (event.key.code == sf::Keyboard::C) {
                    colorIndex = (colorIndex + 1) % 4; // Cycle 0, 1, 2, 3
                    player.changeColor(colors[colorIndex]);
                }
            }
        }

        
        float dt = clock.restart().asSeconds();
        player.update(dt);
        window.clear(sf::Color::Black);
        player.Draw(window);
        window.display();
    }

    return 0;
}