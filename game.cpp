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

    // --- NEW: Load AND Resize Custom Cursor ---
    // This fixes the "too big" issue and aligns the click point
    sf::Texture cursorTex;
    if (cursorTex.loadFromFile("img/cursor.png")) {
        // 1. Create a sprite for the cursor
        sf::Sprite cursorSprite(cursorTex);
        
        // 2. Target a standard cursor size (e.g., 32x32 pixels)
        float targetSize = 32.0f; 
        float scaleX = targetSize / cursorTex.getSize().x;
        float scaleY = targetSize / cursorTex.getSize().y;
        cursorSprite.setScale(scaleX, scaleY);

        // 3. Draw resized cursor onto a Render Texture
        sf::RenderTexture rt;
        if (rt.create(32, 32)) {
            rt.clear(sf::Color::Transparent);
            rt.draw(cursorSprite);
            rt.display();

            // 4. Convert back to Image for the Cursor object
            cursorImage = rt.getTexture().copyToImage();
            
            // 5. Set Cursor (Hotspot at 0,0 - top left)
            if (gameCursor.loadFromPixels(cursorImage.getPixelsPtr(), {32, 32}, {0, 0})) {
                window.setMouseCursor(gameCursor);
            } else {
                std::cerr << "Failed to create cursor from pixels" << std::endl;
            }
        }
    } else {
        std::cerr << "Error loading img/cursor.png" << std::endl;
    }
    // -------------------------------

    initUI();
    loadScreenBg("start_screen.png"); 

    --- AUDIO INIT ---
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

    // Initialize final score text
    finalScoreText.setFont(font);
    finalScoreText.setCharacterSize(40);
    finalScoreText.setFillColor(sf::Color::White);
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
    // Standard Menu Buttons
    btnStart = new Button(250, 400, 400, 133, "PLAY", font);
    btnStart->setTexture("img/button.png");

    btnRetry = new Button(220, 375, 400, 133, "TRY AGAIN", font);
    btnRetry->setTexture("img/button.png");

    btnMenu = new Button(220, 475, 400, 133, "MAIN MENU", font);
    btnMenu->setTexture("img/button.png");

    btnNextLevel = new Button(220, 375, 400, 133, "CONTINUE", font);
    btnNextLevel->setTexture("img/button.png");

    // Level Select Grid Buttons
    for (int i = 0; i < 8; i++) {
        float x = 70 + (i % 4) * 150; 
        float y = 150 + (i / 4) * 150; 
        std::string label = std::to_string(i + 1);
        
        Button* lvlBtn = new Button(x, y, 200, 200, label, font);
        lvlBtn->setTexture("img/buttonlevel.png"); // Different texture for levels
        
        levelButtons.push_back(lvlBtn);
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
                    
                    // Reset score for new game if needed, or keep accumulating
                    // totalScore = 0; // Uncomment if score should reset per level attempt

                    if (currentLevelObject) delete currentLevelObject;
                    currentLevelObject = new Level(window);
                    currentLevelObject->loadLevel(currentLevelIdx, this);
                    
                    // Store score before level starts to revert if lost
                    scoreBeforeLevel = totalScore;

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
                // Revert score since level was lost
                totalScore = scoreBeforeLevel;

                currentLevelObject->loadLevel(currentLevelIdx, this);
                state = PLAYING;
                
                // Audio Transition: WinLose -> Game
                bgmWinLose.stop();
                bgmGame.play();
            }
            if (btnMenu->isClicked(event, window)) {
                // Revert score
                totalScore = scoreBeforeLevel;

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
                
                // Update score snapshot for next level
                scoreBeforeLevel = totalScore;

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
            // Display score on Game Over screen (using score before level since current attempt failed)
            finalScoreText.setString("Score: " + std::to_string(scoreBeforeLevel));
            // Center text
            sf::FloatRect textRect = finalScoreText.getLocalBounds();
            finalScoreText.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
            finalScoreText.setPosition(400, 200); // Center of screen
            
            window.draw(finalScoreText);

            btnRetry->draw(window);
            btnMenu->draw(window);
        }
        else if (state == WIN) {
            // Display score on Win screen
            finalScoreText.setString("Score: " + std::to_string(totalScore));
             // Center text
            sf::FloatRect textRect = finalScoreText.getLocalBounds();
            finalScoreText.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
            finalScoreText.setPosition(400, 300); // Center of screen

            window.draw(finalScoreText);

            if (currentLevelIdx < 8) btnNextLevel->draw(window);
            btnMenu->draw(window);
        }
    }

    window.display();
}