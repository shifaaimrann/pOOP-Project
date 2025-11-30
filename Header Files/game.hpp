#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // Added for Audio
#include <vector>
#include "Level.hpp"
#include "Button.hpp"
class Level;

enum GameState { MENU, LEVEL_SELECT, PLAYING, GAME_OVER, WIN };

class Game {
public:
    Game();
    ~Game(); // destructor to clean up pointers
    void setScore();
    int getScore() const;

    void run();

private:
    int totalScore;
    void processEvents();
    void update(float dt);
    void render();

    void initUI();
    void loadScreenBg(const std::string& filename); 

    sf::RenderWindow window;
    GameState state;
    
    Level* currentLevelObject; 

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    sf::Font font;
    
    Button* btnStart;
    Button* btnRetry;
    Button* btnMenu;
    Button* btnNextLevel; 
    
    std::vector<Button*> levelButtons;

    int unlockedLevels;
    int currentLevelIdx;

    // --- Audio Members ---
    sf::Music bgmMenu;
    sf::Music bgmGame;
    sf::Music bgmWinLose;
};