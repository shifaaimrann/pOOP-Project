// Game.cpp
#include "Header Files/game.hpp"
#include <iostream>

Game::Game() : window(sf::VideoMode(800, 600), "Miffy Switch"), totalScore(0) {
    window.setFramerateLimit(60);
    
    if(!font.loadFromFile("fonts/font.ttf")) {
        std::cerr << "Error loading fonts/font.ttf" << std::endl;
    }

    state = MENU;
    unlockedLevels = 8; 
    currentLevelObject = nullptr; 

    initUI();
    loadScreenBg("start_screen.png"); 

    // --- AUDIO INIT ---
    // Background music for Menu and Level Select
    if (!bgmMenu.openFromFile("audio/mainmenuandlevels.mp3")) {
        std::cerr << "Error loading audio/mainmenuandlevels.mp3" << std::endl;
    }
    bgmMenu.setLoop(true);

    // Background music for Gameplay
    if (!bgmGame.openFromFile("audio/whilePlaying.mp3")) {
        std::cerr << "Error loading audio/whilePlaying.mp3" << std::endl;
    }
    bgmGame.setLoop(true);

    // Music for Win/Lose Screens
    if (!bgmWinLose.openFromFile("audio/game_lose_win.mp3")) {
        std::cerr << "Error loading audio/game_lose_win.mp3" << std::endl;
    }
    bgmWinLose.setLoop(true);

    // Start by playing menu music
    bgmMenu.play();
}

void Game::setScore(){ totalScore++; }
int Game::getScore() const { return totalScore; }

Game::~Game() {
    delete btnStart;
    delete btnRetry;
    delete btnMenu;
    delete btnNextLevel;
    
    for (auto* btn : levelButtons) delete btn;
    levelButtons.clear();

    if (currentLevelObject) delete currentLevelObject;
}

void Game::initUI() {
    btnStart = new Button(300, 450, 200, 60, "PLAY", font);
    btnRetry = new Button(250, 400, 300, 60, "TRY AGAIN", font);
    btnMenu = new Button(250, 500, 300, 60, "MAIN MENU", font);
    btnNextLevel = new Button(250, 400, 300, 60, "CONTINUE", font);

    for (int i = 0; i < 8; i++) {
        float x = 100 + (i % 4) * 150; 
        float y = 200 + (i / 4) * 150; 
        std::string label = std::to_string(i + 1);
        levelButtons.push_back(new Button(x, y, 100, 100, label, font));
    }
}

void Game::loadScreenBg(const std::string& filename) {
    std::string path = "img/screens/" + filename;
    if (!bgTexture.loadFromFile(path)) {
        std::cerr << "Missing image " << path << std::endl;
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

        // --- MENU STATE ---
        if (state == MENU) {
            if (btnStart->isClicked(event, window)) {
                state = LEVEL_SELECT;
                loadScreenBg("level_select.png");
                // Note: bgmMenu continues playing in Level Select
            }
        }
        // --- LEVEL SELECT STATE ---
        else if (state == LEVEL_SELECT) {
            for (int i = 0; i < 8; i++) {
                if (i < unlockedLevels && levelButtons[i]->isClicked(event, window)) {
                    currentLevelIdx = i + 1;
                    
                    if (currentLevelObject) delete currentLevelObject;
                    currentLevelObject = new Level(window);
                    currentLevelObject->loadLevel(currentLevelIdx, this);
                    
                    state = PLAYING;
                    
                    // Audio Transition: Menu -> Game
                    bgmMenu.stop();
                    bgmGame.play();
                }
            }
        }
        // --- PLAYING STATE ---
        else if (state == PLAYING) {
            if (currentLevelObject) currentLevelObject->handleInput(event);
        }
        // --- GAME OVER STATE ---
        else if (state == GAME_OVER) {
            if (btnRetry->isClicked(event, window)) {
                currentLevelObject->loadLevel(currentLevelIdx, this);
                state = PLAYING;
                
                // Audio Transition: WinLose -> Game
                bgmWinLose.stop();
                bgmGame.play();
            }
            if (btnMenu->isClicked(event, window)) {
                state = MENU;
                loadScreenBg("start_screen.png");
                
                // Audio Transition: WinLose -> Menu
                bgmWinLose.stop();
                bgmMenu.play();
            }
        }
        // --- WIN STATE ---
        else if (state == WIN) {
            if (currentLevelIdx < 8 && btnNextLevel->isClicked(event, window)) {
                currentLevelIdx++;
                if (currentLevelIdx > unlockedLevels) unlockedLevels = currentLevelIdx;
                
                delete currentLevelObject;
                currentLevelObject = new Level(window);
                currentLevelObject->loadLevel(currentLevelIdx, this);
                state = PLAYING;
                
                // Audio Transition: WinLose -> Game
                bgmWinLose.stop();
                bgmGame.play();
            }
            
            if (btnMenu->isClicked(event, window)) {
                state = MENU;
                loadScreenBg("start_screen.png");
                
                // Audio Transition: WinLose -> Menu
                bgmWinLose.stop();
                bgmMenu.play();
            }
        }
    }
}

void Game::update(float dt) {
    if (state == PLAYING && currentLevelObject) {
        currentLevelObject->update(dt, this);

        // CHECK FOR GAME OVER
        if (currentLevelObject->hasLost()) {
            state = GAME_OVER;
            loadScreenBg("game_over.png"); 
            
            // Audio Transition: Game -> Lose
            bgmGame.stop();
            bgmWinLose.play();
        }
        
        // CHECK FOR WIN
        if (currentLevelObject->hasWon()) {
            state = WIN;
            
            if (currentLevelIdx == unlockedLevels && unlockedLevels < 8) {
                unlockedLevels++;
            }
            
            if (currentLevelIdx == 8) {
                loadScreenBg("win_game.png");
            } 
            else {
                loadScreenBg("level_win.png");
            }
            
            // Audio Transition: Game -> Win
            bgmGame.stop();
            bgmWinLose.play();
        }
    }
}

void Game::render() {
    window.clear();

    if (state == PLAYING) {
        if (currentLevelObject) currentLevelObject->draw();
    }
    else {
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