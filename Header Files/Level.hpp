// Header Files/Level.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "PlayerSprite.hpp"
#include "Obstacle.hpp"
#include "ColorWheel.hpp"
#include "star.hpp"
#include "HealthBar.hpp"
#include "HealthPotion.hpp"
class Game;
class PlayerSprite;


class Level {
public:
    Level(sf::RenderWindow& win); 
    ~Level(); // destructor cleans up obstacles

    void loadLevel(int levelNum, Game*);
    void update(float dt, Game*);
    void handleInput(sf::Event& event);
    void draw();
    
    // helpers for game manager
    bool hasLost() const { return isGameOver; }
    bool hasWon() const { return isWin; }

private:
    sf::RenderWindow& window;
    
    // raw pointers for manual memory management
    PlayerSprite* player;
    HealthPotion* healthPotion;
    
    // vector of pointers for mixed obstacle types
    std::vector<Obstacle*> obstacles;
    std::vector<Star*> stars;
    
    // infinite background system
    sf::Texture bgTexture;
    sf::Sprite bgSprite1; 
    sf::Sprite bgSprite2; 

    // ui elements
    sf::Font font;
    sf::Text scoreText;
    sf::Sprite scoreIcon;
    sf::Texture starTexture;

    bool waitingToStart;
    bool isGameOver;
    bool isWin;
    int currentLevel;
    float finishLineY;
    bool potionSpawned;
    
    // handles infinite scrolling
    void moveWorld(float offset);
};