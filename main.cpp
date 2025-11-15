#include <SFML/Graphics.hpp>
#include "Header Files/PlayerSprite.hpp"
#include <vector>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Muffy Switch =＾● ⋏ ●＾=");
    window.setFramerateLimit(60);
    ///////////////this draws the backgrund!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    const sf::Color lightOrange(255, 178, 102);
    const sf::Color darkOrange(255, 140, 51);
    const sf::Color borderBgColor(75, 0, 130); 
    const sf::Color segmentColor(55, 0, 90, 200);  
    const sf::Color lineHighlight(138, 43, 226); 
    const float scrollSpeed = 80.f; 
    const float borderWidth = 48.f;
    const float stripeHeight = 60.f;
    const float segmentHeight = 60.f;
    const int numStripes = 12;
    const int numSegments = 12;
    std::vector<sf::RectangleShape> stripes(numStripes);
    std::vector<sf::RectangleShape> leftSegments(numSegments);
    std::vector<sf::RectangleShape> rightSegments(numSegments);
    for (int i = 0; i < numStripes; ++i) {
        stripes[i].setSize(sf::Vector2f(800, stripeHeight));
        stripes[i].setFillColor((i % 2 == 0) ? lightOrange : darkOrange);
        stripes[i].setPosition(0, i * stripeHeight);
    }
    sf::RectangleShape leftBorderBg(sf::Vector2f(borderWidth, 600));
    leftBorderBg.setFillColor(borderBgColor);
    leftBorderBg.setPosition(0, 0);
    sf::RectangleShape rightBorderBg(sf::Vector2f(borderWidth, 600));
    rightBorderBg.setFillColor(borderBgColor);
    rightBorderBg.setPosition(800 - borderWidth, 0);
    sf::RectangleShape leftLine(sf::Vector2f(2, 600));
    leftLine.setFillColor(lineHighlight);
    leftLine.setPosition(borderWidth, 0);
    sf::RectangleShape rightLine(sf::Vector2f(2, 600));
    rightLine.setFillColor(lineHighlight);
    rightLine.setPosition(800 - borderWidth - 2, 0);
    for (int i = 0; i < numSegments; ++i) {
        leftSegments[i].setSize(sf::Vector2f(borderWidth - 8, segmentHeight - 8));
        leftSegments[i].setFillColor(segmentColor);
        leftSegments[i].setPosition(4, (i * segmentHeight) + 4);
        rightSegments[i].setSize(sf::Vector2f(borderWidth - 8, segmentHeight - 8));
        rightSegments[i].setFillColor(segmentColor);
        rightSegments[i].setPosition((800 - borderWidth) + 4, (i * segmentHeight) + 4);
    }
    // ----------------------------------------------------------------------------------- 
    PlayerSprite player(400.0f, 300.0f);//calling player constrtuctotr with inital position
    sf::Clock clock;
    sf::Color colors[] = {//all the colors of the game the ball can take
        sf::Color(200, 0, 255), //pink
        sf::Color::Blue,
        sf::Color::Yellow,
        sf::Color::White
    };
    int colorIndex = 0;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) player.jump();
                if (event.key.code == sf::Keyboard::C) {
                    colorIndex = (colorIndex + 1) % 4;
                    player.changeColor(colors[colorIndex]);
                }
            }
        }
        float dt = clock.restart().asSeconds();
        player.update(dt);
        for (int i = 0; i < numStripes; ++i) {
            stripes[i].move(0, scrollSpeed * dt); 
            if (stripes[i].getPosition().y > 600) { 
                stripes[i].setPosition(0, stripes[i].getPosition().y - (numStripes * stripeHeight));
            }
        }
        for (int i = 0; i < numSegments; ++i) {
            leftSegments[i].move(0, scrollSpeed * dt);
            if (leftSegments[i].getPosition().y > 600) {
                leftSegments[i].setPosition(4, leftSegments[i].getPosition().y - (numSegments * segmentHeight));
            }
            rightSegments[i].move(0, scrollSpeed * dt); 
            if (rightSegments[i].getPosition().y > 600) { 
                rightSegments[i].setPosition((800 - borderWidth) + 4, rightSegments[i].getPosition().y - (numSegments * segmentHeight));
            }
        }

        window.clear();
        for (const auto& stripe : stripes) {
            window.draw(stripe);
        }

        window.draw(leftBorderBg);
        window.draw(rightBorderBg);
        for (const auto& segment : leftSegments) {
            window.draw(segment);
        }
        for (const auto& segment : rightSegments) {
            window.draw(segment);
        }
        window.draw(leftLine);
        window.draw(rightLine);

        player.Draw(window);

        window.display(); //final window display
    }

    return 0;
}