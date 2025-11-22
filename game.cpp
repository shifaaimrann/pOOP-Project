// Game.cpp
#include "Header Files/game.hpp"
#include <iostream>

Game::Game() : window(sf::VideoMode(800, 600), "Miffy Switch") {
    window.setFramerateLimit(60);
    
    font.loadFromFile("fonts/font.ttf");

    state = MENU;
    unlockedLevels = 1; 
    currentLevelObject = nullptr; // no level loaded yet

    initUI();
    loadScreenBg("start_screen.png"); 
}

Game::~Game() {
    // manually delete all pointers to prevent leaks
    delete btnStart;
    delete btnRetry;
    delete btnMenu;
    delete btnNextLevel;
    
    for (auto* btn : levelButtons) delete btn;
    levelButtons.clear();

    if (currentLevelObject) delete currentLevelObject;
}

void Game::initUI() {
    // main menu buttons
    btnStart = new Button(300, 450, 200, 60, "PLAY", font);
    
    // game over and win buttons
    btnRetry = new Button(250, 400, 300, 60, "TRY AGAIN", font);
    btnMenu = new Button(250, 500, 300, 60, "MAIN MENU", font);
    btnNextLevel = new Button(250, 400, 300, 60, "CONTINUE", font);

    // generate grid of 8 level buttons
    for (int i = 0; i < 8; i++) {
        float x = 100 + (i % 4) * 150; 
        float y = 200 + (i / 4) * 150; 
        std::string label = std::to_string(i + 1);
        levelButtons.push_back(new Button(x, y, 100, 100, label, font));
    }
}

void Game::loadScreenBg(const std::string& filename) {
    // attempts to load image from screens folder
    std::string path = "img/screens/" + filename;
    if (!bgTexture.loadFromFile(path)) {
        std::cout << "missing image " << path << std::endl;
    }
    bgSprite.setTexture(bgTexture);
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        processEvents();
        float dt = clock.restart().asSeconds();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();

        // menu state
        if (state == MENU) {
            if (btnStart->isClicked(event, window)) {
                state = LEVEL_SELECT;
                loadScreenBg("level_select.png");
            }
        }
        // level select state
        else if (state == LEVEL_SELECT) {
            for (int i = 0; i < 8; i++) {
                // only allow unlocked levels
                if (i < unlockedLevels && levelButtons[i]->isClicked(event, window)) {
                    currentLevelIdx = i + 1;
                    
                    // delete old level if it exists
                    if (currentLevelObject) delete currentLevelObject;
                    
                    // create new level
                    currentLevelObject = new Level(window);
                    currentLevelObject->loadLevel(currentLevelIdx);
                    
                    state = PLAYING;
                }
            }
        }
        // playing state
        else if (state == PLAYING) {
            if (currentLevelObject) currentLevelObject->handleInput(event);
        }
        // game over state
        else if (state == GAME_OVER) {
            if (btnRetry->isClicked(event, window)) {
                // reload same level
                currentLevelObject->loadLevel(currentLevelIdx);
                state = PLAYING;
            }
            if (btnMenu->isClicked(event, window)) {
                state = MENU;
                loadScreenBg("start_screen.png");
            }
        }
        // win state
        else if (state == WIN) {
            // next level button logic
            if (currentLevelIdx < 8 && btnNextLevel->isClicked(event, window)) {
                currentLevelIdx++;
                // unlock next level if it is new
                if (currentLevelIdx > unlockedLevels) unlockedLevels = currentLevelIdx;
                
                // load the next level
                delete currentLevelObject;
                currentLevelObject = new Level(window);
                currentLevelObject->loadLevel(currentLevelIdx);
                state = PLAYING;
            }
            
            if (btnMenu->isClicked(event, window)) {
                state = MENU;
                loadScreenBg("start_screen.png");
            }
        }
    }
}

void Game::update(float dt) {
    if (state == PLAYING && currentLevelObject) {
        currentLevelObject->update(dt);

        // check for game over
        if (currentLevelObject->hasLost()) {
            state = GAME_OVER;
            loadScreenBg("game_over.png");
        }
        
        // check for win
        if (currentLevelObject->hasWon()) {
            state = WIN;
            
            // unlock level if needed
            if (currentLevelIdx == unlockedLevels && unlockedLevels < 8) {
                unlockedLevels++;
            }
            
            // load custom win screen for this specific level
            if (currentLevelIdx == 8) {
                loadScreenBg("win_game.png"); // final win
            } else {
                loadScreenBg("win_lvl" + std::to_string(currentLevelIdx) + ".png");
            }
        }
    }
}

void Game::render() {
    window.clear();

    if (state == PLAYING) {
        // level draws its own background and objects
        if (currentLevelObject) currentLevelObject->draw();
    }
    else {
        // draw the menu background image
        window.draw(bgSprite);
        
        if (state == MENU) {
            btnStart->draw(window);
        }
        else if (state == LEVEL_SELECT) {
            for (int i = 0; i < 8; i++) levelButtons[i]->draw(window);
        }
        else if (state == GAME_OVER) {
            btnRetry->draw(window);
            btnMenu->draw(window);
        }
        else if (state == WIN) {
            if (currentLevelIdx < 8) btnNextLevel->draw(window);
            btnMenu->draw(window);
        }
    }

    window.display();
}