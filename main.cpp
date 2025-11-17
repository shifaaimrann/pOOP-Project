#include <SFML/Graphics.hpp>
#include "Header Files/PlayerSprite.hpp" // correct path
#include "Header Files/star.hpp"        // correct path
#include "Header Files/ColorWheel.hpp"  
#include <vector>
#include <cmath>
#include <string>     // for std::to_string
#include <iostream>   // for std::cerr

// a simple structure to hold our trail particles
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Time     lifetime;
    sf::RectangleShape shape;
    sf::Color    color;
};

// helper function to create pretty rainbow colors
sf::Color getRainbowColor(float time) {
    float r = std::sin(time * 2.0f) * 0.5f + 0.5f;
    float g = std::sin(time * 2.0f + 2.0f * 3.14159f / 3.0f) * 0.5f + 0.5f;
    float b = std::sin(time * 2.0f + 4.0f * 3.14159f / 3.0f) * 0.5f + 0.5f;
    return sf::Color(r * 255, g * 255, b * 255);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Color Switch");
    window.setFramerateLimit(60);

    //--- background setup ---
    const sf::Color lightOrange(255, 178, 102);
    const sf::Color darkOrange(255, 140, 51);
    const sf::Color borderBgColor(75, 0, 130); 
    const sf::Color segmentColor(55, 0, 90, 200);   
    const sf::Color lineHighlight(138, 43, 226); 
    const float scrollSpeed = 80.f; // positive for moving down
    const float borderWidth = 48.f;
    const float stripeHeight = 60.f;
    const float segmentHeight = 60.f;
    const int numStripes = 12; // 12 * 60 = 720 (extra for wrapping)
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
    //--- end background setup ---

    //--- create game objects ---
    PlayerSprite player(400.0f, 300.0f);
    Star star(400.0f, 150.0f); // create a star object
    ColorWheel wheel(400.f, 150.f, 100.f, 15.f, 60.f); // x, y, radius, thickness, speed

    //--- particle trail setup ---
    std::vector<Particle> particles;
    sf::Clock particleTimer; 
    float particleSpawnTime = 0.02f; 
    float particleHue = 0.0f; 

    //--- score ui setup ---
    int starCount = 0;
    sf::Font font;
    if (!font.loadFromFile("fonts/font.ttf")) { // fixed path
        std::cerr << "error: could not load 'fonts/font.ttf'" << std::endl;
    }

    sf::Texture scoreStarTexture;
    if (!scoreStarTexture.loadFromFile("img/star.png")) { // path to your star image
        std::cerr << "error: could not load 'img/star.png' for score" << std::endl;
    }
    
    sf::Sprite scoreStarIcon;
    scoreStarIcon.setTexture(scoreStarTexture);
    scoreStarIcon.setScale(0.08f, 0.08f); // small icon
    scoreStarIcon.setPosition(15, 10); // top-left corner
    
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setString("0");
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(55, 8); // next to the icon

    //--- clocks and color cycle ---
    sf::Clock clock;
    sf::Clock gameTimeClock; // for rainbow particles

    sf::Color colors[] = {
        sf::Color(255, 0, 255), // 0: pink
        sf::Color::Blue,        // 1: blue
        sf::Color::Yellow,      // 2: yellow
        sf::Color::White        // 3: white (for grayscale)
    };
    int colorIndex = 0; // start at pink

    //--- main game loop ---
    while (window.isOpen()) {
        
        // --- 1. handle events ---
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
                    colorIndex = (colorIndex + 1) % 4; // cycle 0, 1, 2, 3
                    player.changeColor(colors[colorIndex]);
                }
            }
        }

        // --- 2. update game logic ---
        float dt = clock.restart().asSeconds();
        
        player.update(dt);
        star.update(dt); // update the star (so it can be 'collected')

        //--- update particle trail ---
        if (particleTimer.getElapsedTime().asSeconds() > particleSpawnTime) {
            particleTimer.restart();

            Particle p;
            p.position = player.getPosition();

            float wiggle = (std::rand() % 100) / 100.0f - 0.5f; // random value from -0.5 to +0.5
            p.velocity = sf::Vector2f(wiggle * 50.f, 20.f); // wiggle horizontally, drift down
            
            p.lifetime = sf::seconds(0.7f);
            p.shape.setSize(sf::Vector2f(10, 10));
            p.shape.setOrigin(5, 5); // center the particle
            p.shape.setPosition(p.position);
            p.color = getRainbowColor(gameTimeClock.getElapsedTime().asSeconds());
            p.shape.setFillColor(p.color);

            particles.push_back(p);
        }

        // update all living particles
        for (int i = particles.size() - 1; i >= 0; --i) {
            particles[i].lifetime -= sf::seconds(dt);

            if (particles[i].lifetime <= sf::Time::Zero) {
                particles.erase(particles.begin() + i); // remove dead particles
            } else {
                particles[i].position += particles[i].velocity * dt; // move it
                particles[i].shape.setPosition(particles[i].position);

                // fade it out
                float lifeRatio = particles[i].lifetime.asSeconds() / 0.7f;
                sf::Color c = particles[i].color;
                c.a = static_cast<sf::Uint8>(lifeRatio * 255);
                particles[i].shape.setFillColor(c);
            }
        }

        //--- collision check ---
        if (!star.isCollected()) { // only check if star is on-screen
            if (player.getBounds().intersects(star.getBounds())) {
                star.collect(); // set star's internal state to 'collected'
                starCount++; // increase score
                scoreText.setString(std::to_string(starCount)); // update score text
            }
        }
        if (player.getBounds().intersects(wheel.getBounds())) {
            // this is a simple "AABB" collision check (a box)
            // in the real game, you'd do a per-pixel or arc check here
            // and compare player.getColor() to wheel.getColorAtAngle()
        }
        //--- update background scroll (downwards) ---
        for (int i = 0; i < numStripes; ++i) {
            stripes[i].move(0, scrollSpeed * dt); 
            if (stripes[i].getPosition().y > 600) { // if off-screen bottom
                stripes[i].setPosition(0, stripes[i].getPosition().y - (numStripes * stripeHeight)); // wrap to top
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

        // --- 3. draw everything ---
        // --- 3. draw everything ---
        window.clear(); // clear with default (black)

        // draw background first
        for (const auto& stripe : stripes) window.draw(stripe);
        window.draw(leftBorderBg);
        window.draw(rightBorderBg);
        for (const auto& segment : leftSegments) window.draw(segment);
        for (const auto& segment : rightSegments) window.draw(segment);
        window.draw(leftLine);
        window.draw(rightLine);

        // draw particles (behind player)
        for (const auto& p : particles) {
            window.draw(p.shape);
        }

        // draw game objects
        wheel.Draw(window);  // draw the wheel
        player.Draw(window);
        star.Draw(window);   // draw the star (it will be inside the wheel)

        // draw ui last (so it's on top)
        window.draw(scoreStarIcon);
        window.draw(scoreText);
        
        window.display(); // show the new frame// show the new frame
    }

    return 0;
}