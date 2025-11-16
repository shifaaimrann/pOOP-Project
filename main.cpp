#include <SFML/Graphics.hpp>
#include "Header Files/PlayerSprite.hpp"
#include <vector>
#include <cmath>

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Time  lifetime;
    sf::RectangleShape shape;
    sf::Color color;
};

sf::Color getRainbowColor(float time) {
    float r = std::sin(time * 2.0f) * 0.5f + 0.5f;
    float g = std::sin(time * 2.0f + 2.0f * 3.14159f / 3.0f) * 0.5f + 0.5f;
    float b = std::sin(time * 2.0f + 4.0f * 3.14159f / 3.0f) * 0.5f + 0.5f;
    return sf::Color(r * 255, g * 255, b * 255);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Miffy Switch (=^=)");
    window.setFramerateLimit(60);

    //--- background setup!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
    //--- ---------------------------------------------------------------------------------------------

    PlayerSprite player(400.0f, 300.0f);

    //-rainbow trail           
    std::vector<Particle> particles;
    sf::Clock particleTimer; 
    float particleSpawnTime = 0.02f; 
    float particleHue = 0.0f; 

    sf::Clock clock;
    sf::Clock gameTimeClock; 

    sf::Color colors[] = {
    sf::Color(255, 0, 255), //pink
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

    if (particleTimer.getElapsedTime().asSeconds() > particleSpawnTime) {
    particleTimer.restart();
    particleHue += 0.1f; //advance the rainbow

    Particle p;
    p.position = player.getPosition();

    float wiggle = (std::rand() % 100) / 100.0f - 0.5f; //-0.5 to +0.5
    p.velocity = sf::Vector2f(wiggle * 50.f, 20.f); //wiggly, and moves down slightly

    p.lifetime = sf::seconds(0.7f); //how long the particle lives
    p.shape.setSize(sf::Vector2f(10, 10));
    p.shape.setOrigin(5, 5);
    p.shape.setPosition(p.position);
    p.color = getRainbowColor(gameTimeClock.getElapsedTime().asSeconds());
    p.shape.setFillColor(p.color);

    particles.push_back(p);
    }

    for (int i = particles.size() - 1; i >= 0; --i) {
    particles[i].lifetime -= sf::seconds(dt);

    if (particles[i].lifetime <= sf::Time::Zero) { //if particle is dead, it is thus removed
    particles.erase(particles.begin() + i);
    } else {
    particles[i].position += particles[i].velocity * dt; //moving
    particles[i].shape.setPosition(particles[i].position);

    float lifeRatio = particles[i].lifetime.asSeconds() / 0.7f; //fading
    sf::Color c = particles[i].color;
    c.a = static_cast<sf::Uint8>(lifeRatio * 255);
    particles[i].shape.setFillColor(c);
    }
    }

    for (int i = 0; i < numStripes; ++i) {
    stripes[i].move(0, scrollSpeed * dt); //move down
    if (stripes[i].getPosition().y > 600) { //if off-screen bottom
    stripes[i].setPosition(0, stripes[i].getPosition().y - (numStripes * stripeHeight)); //wrap to top
    }
    }
    for (int i = 0; i < numSegments; ++i) {
    leftSegments[i].move(0, scrollSpeed * dt); //move down
    if (leftSegments[i].getPosition().y > 600) {
    leftSegments[i].setPosition(4, leftSegments[i].getPosition().y - (numSegments * segmentHeight));
    }
    rightSegments[i].move(0, scrollSpeed * dt); //move down
    if (rightSegments[i].getPosition().y > 600) {
    rightSegments[i].setPosition((800 - borderWidth) + 4, rightSegments[i].getPosition().y - (numSegments * segmentHeight));
    }
    }

    window.clear();

    for (const auto& stripe : stripes) window.draw(stripe);
    window.draw(leftBorderBg);
    window.draw(rightBorderBg);
    for (const auto& segment : leftSegments) window.draw(segment);
    for (const auto& segment : rightSegments) window.draw(segment);
    window.draw(leftLine);
    window.draw(rightLine);

    for (const auto& p : particles) window.draw(p.shape);

    player.Draw(window); //
    window.display();
    }

 return 0;
}