// Header Files/Game.hpp
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
    int scoreBeforeLevel; // To revert score on loss
    void processEvents();
    void update(float dt);
    void render();

    void initUI();
    // loads background for menus like start or win
    void loadScreenBg(const std::string& filename); 

    sf::RenderWindow window;
    
    // --- NEW: Custom Cursor Members ---
    sf::Cursor gameCursor;
    sf::Image cursorImage;
    // ----------------------------------

    GameState state;
    
    // pointer allows us to delete and recreate levels
    Level* currentLevelObject; 

    // background for menus
    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    sf::Font font;
    sf::Text finalScoreText; // Text to display score on win/lose screens
    
    // ui pointers
    Button* btnStart;
    Button* btnRetry;
    Button* btnMenu;
    Button* btnNextLevel; // continues to next level
    
    // list of pointers for the level grid
    std::vector<Button*> levelButtons;

    int unlockedLevels;
    int currentLevelIdx;

    // --- Audio Members ---
    sf::Music bgmMenu;
    sf::Music bgmGame;
    sf::Music bgmWinLose;
};