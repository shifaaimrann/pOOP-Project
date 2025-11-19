#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "PlayerSprite.hpp"
#include "Obstacle.hpp"
#include "ColorWheel.hpp"
#include "star.hpp"
#include "HealthBar.hpp"

class Level {
public:
    Level(sf::RenderWindow& win); 
    ~Level();

    void loadLevel(int levelNum);
    void update(float dt);
    void handleInput(sf::Event& event);
    void draw();
    
    // resets the current level
    void restart();

private:
    sf::RenderWindow& window;
    
    // game objects
    PlayerSprite* player;
    HealthBar* healthBar;
    
    std::vector<Obstacle*> obstacles;
    std::vector<Star*>     stars;
    
    // --- background elements ---
    std::vector<sf::RectangleShape> stripes;
    std::vector<sf::RectangleShape> leftSegments;
    std::vector<sf::RectangleShape> rightSegments;
    sf::RectangleShape leftBorderBg;
    sf::RectangleShape rightBorderBg;
    sf::RectangleShape leftLine;
    sf::RectangleShape rightLine;

    // ui
    sf::Font font;
    sf::Text gameOverText;
    sf::Text winText;
    sf::Text scoreText;
    sf::Sprite scoreIcon;
    sf::Texture starTexture;

    // state
    bool isGameOver;
    bool isWin;
    int  currentLevel;
    float finishLineY;
    
    // helper for background elements
    void moveWorld(float offset);
};