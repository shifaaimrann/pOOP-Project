// Header Files/Game.hpp
#pragma once
#include <SFML/Graphics.hpp>
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
    // loads background for menus like start or win
    void loadScreenBg(const std::string& filename); 

    sf::RenderWindow window;
    GameState state;
    
    // pointer allows us to delete and recreate levels
    Level* currentLevelObject; 

    // background for menus
    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    sf::Font font;
    
    // ui pointers
    Button* btnStart;
    Button* btnRetry;
    Button* btnMenu;
    Button* btnNextLevel; // continues to next level
    
    // list of pointers for the level grid
    std::vector<Button*> levelButtons;

    int unlockedLevels;
    int currentLevelIdx;
};