#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // Added for Audio
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
    ~Level(); 

    void loadLevel(int levelNum, Game*);
    void update(float dt, Game*);
    void handleInput(sf::Event& event);
    void draw();
    
    bool hasLost() const { return isGameOver; }
    bool hasWon() const { return isWin; }

private:
    sf::RenderWindow& window;
    PlayerSprite* player;
    HealthPotion* healthPotion;
    
    std::vector<Obstacle*> obstacles;
    std::vector<Star*> stars;
    
    sf::Texture bgTexture;
    sf::Sprite bgSprite1; 
    sf::Sprite bgSprite2; 

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
    
    // Death Sequence Variables
    bool isDying;
    float deathTimer;
    float shakeTimer;

    // Audio for Level
    sf::SoundBuffer damageBuffer;
    sf::Sound damageSound;
    sf::SoundBuffer dyingBuffer;
    sf::Sound dyingSound;

    void moveWorld(float offset);
};