// Level.cpp
#include "Header Files/Level.hpp"
#include <iostream>
#include "Header Files/game.hpp"

Level::Level(sf::RenderWindow& win) : window(win), healthPotion(nullptr) {
    player = nullptr;
    potionSpawned = false;

    // Load Fonts and Textures
    if (!font.loadFromFile("fonts/font.ttf")) {
        std::cerr << "Error loading font.ttf" << std::endl;
    }
    if (!starTexture.loadFromFile("img/star.png")) {
        std::cerr << "Error loading star.png" << std::endl;
    }

    // Score setup
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setPosition(55, 38);
    
    scoreIcon.setTexture(starTexture);
    scoreIcon.setScale(0.08f, 0.08f);
    scoreIcon.setPosition(15, 40);

    // --- AUDIO SETUP ---
    // Load sound buffers (small files loaded into memory)
    if (!damageBuffer.loadFromFile("audio/healthbarreduce.mp3")) {
        std::cerr << "Error loading audio/healthbarreduce.mp3" << std::endl;
    }
    damageSound.setBuffer(damageBuffer);

    if (!dyingBuffer.loadFromFile("audio/whileDying.mp3")) {
        std::cerr << "Error loading audio/whileDying.mp3" << std::endl;
    }
    dyingSound.setBuffer(dyingBuffer);
    // -------------------

    isGameOver = false;
    isWin = false;
    currentLevel = 1;
    isDying = false;
    deathTimer = 0.f;
    shakeTimer = 0.f;
}

Level::~Level() {
    if (player) delete player;
    if (healthPotion != nullptr) {
        delete healthPotion;
    }

    for (size_t i = 0; i < obstacles.size(); ++i) delete obstacles[i];
    obstacles.clear();
    
    for (size_t i = 0; i < stars.size(); ++i) delete stars[i];
    stars.clear();
}

void Level::loadLevel(int levelNum, Game* g1) {
    currentLevel = levelNum;
    isGameOver = false;
    isWin = false;
    waitingToStart = true;
    potionSpawned = false;
    
    // Reset death state
    isDying = false;
    deathTimer = 0.f;
    shakeTimer = 0.f;

    // Clear previous level data
    for (size_t i = 0; i < obstacles.size(); ++i) delete obstacles[i];
    obstacles.clear();
    for (size_t i = 0; i < stars.size(); ++i) delete stars[i];
    stars.clear();

    if (healthPotion != nullptr) {
        delete healthPotion;
        healthPotion = nullptr;
    }
    
    if (player) delete player;
    player = new PlayerSprite(400.f, 500.f);

    // Load background
    std::string bgFile = "img/level backgrounds/level" + std::to_string(levelNum) + ".png";
    if (!bgTexture.loadFromFile(bgFile)) {
        bgTexture.loadFromFile("img/level backgrounds/level1.png"); 
    }
    
    bgSprite1.setTexture(bgTexture);
    bgSprite2.setTexture(bgTexture);
    bgSprite1.setPosition(0, 0);
    bgSprite2.setPosition(0, -static_cast<float>(bgTexture.getSize().y));

    // --- LEVEL LAYOUTS ---
    if (levelNum == 1) {
        obstacles.push_back(new ColorWheel(400.f, 200.f, 100.f, 18.f, 90.f));
        stars.push_back(new Star(400.f, 200.f));
        obstacles.push_back(new Paddle(400.f, -100.f, 200.f, 25.f));
        stars.push_back(new Star(400.f, -150.f)); 
        obstacles.push_back(new ColorWheel(400.f, -400.f, 100.f, 18.f, -90.f));
        stars.push_back(new Star(400.f, -400.f));
        finishLineY = -600.f;
    }
    else if (levelNum == 2) {
        obstacles.push_back(new ColorWheel(400.f, 200.f, 100.f, 18.f, 80.f));
        stars.push_back(new Star(400.f, 200.f));
        obstacles.push_back(new Paddle(400.f, -100.f, 200.f, 25.f));
        stars.push_back(new Star(400.f, -130.f));
        obstacles.push_back(new Paddle(400.f, -400.f, 200.f, 25.f));
        stars.push_back(new Star(400.f, -430.f));
        obstacles.push_back(new ColorWheel(400.f, -700.f, 100.f, 18.f, -100.f));
        stars.push_back(new Star(400.f, -700.f));
        obstacles.push_back(new Paddle(400.f, -1000.f, 200.f, 25.f));
        stars.push_back(new Star(400.f, -1030.f));
        finishLineY = -1200.f;
    }
    else if (levelNum == 3) {
        obstacles.push_back(new ColorWheel(400.f, 200.f, 200.f, 18.f, 60.f));
        obstacles.push_back(new ColorWheel(400.f, 200.f, 100.f, 18.f, -50.f));
        stars.push_back(new Star(400.f, 200.f)); 
        obstacles.push_back(new ColorWheel(400.f, -300.f, 180.f, 18.f, 50.f));
        obstacles.push_back(new Paddle(400.f, -300.f, 180.f, 25.f));
        stars.push_back(new Star(400.f, -200.f)); 
        stars.push_back(new Star(400.f, -60.f)); 
        obstacles.push_back(new Paddle(400.f, -700.f, 200.f, 25.f));
        stars.push_back(new Star(400.f, -530.f));
        obstacles.push_back(new ColorWheel(400.f, -900.f, 100.f, 18.f, 90.f));
        stars.push_back(new Star(400.f, -800.f));
        finishLineY = -1100.f;
    }
    else if (levelNum == 4) {
        obstacles.push_back(new ColorSquare(400.f, 150.f, 200.f, 20.f, 70.f));
        stars.push_back(new Star(400.f, 150.f));
        obstacles.push_back(new Paddle(400.f, -150.f, 250.f, 25.f));
        stars.push_back(new Star(400.f, -180.f));
        obstacles.push_back(new ColorSquare(400.f, -450.f, 200.f, 20.f, -90.f));
        stars.push_back(new Star(400.f, -450.f));
        obstacles.push_back(new ColorWheel(400.f, -750.f, 100.f, 18.f, 80.f));
        stars.push_back(new Star(400.f, -750.f));
        finishLineY = -950.f;
    }
    else if (levelNum == 5) {
        obstacles.push_back(new ColorCross(400.f, 150.f, 180.f, 20.f, 60.f));
        stars.push_back(new Star(400.f, 0.f)); 
        obstacles.push_back(new ColorSquare(400.f, -250.f, 220.f, 20.f, -60.f));
        stars.push_back(new Star(400.f, -250.f));
        obstacles.push_back(new Paddle(250.f, -500.f, 150.f, 25.f));
        obstacles.push_back(new Paddle(550.f, -500.f, 150.f, 25.f));
        stars.push_back(new Star(400.f, -500.f));
        obstacles.push_back(new ColorCross(300.f, -750.f, 180.f, 20.f, -100.f));
        stars.push_back(new Star(400.f, -850.f));
        finishLineY = -1000.f;
    }
    else if (levelNum == 6) {
        obstacles.push_back(new ColorSpotWheel(400.f, 150.f, 130.f, 15.f, 16, 40.f));
        stars.push_back(new Star(400.f, 150.f));
        obstacles.push_back(new ColorCross(400.f, -200.f, 160.f, 20.f, 80.f));
        stars.push_back(new Star(400.f, -200.f));
        obstacles.push_back(new ColorSpotWheel(400.f, -550.f, 130.f, 18.f, 12, -70.f));
        stars.push_back(new Star(400.f, -550.f));
        obstacles.push_back(new ColorSquare(400.f, -850.f, 200.f, 20.f, 90.f));
        stars.push_back(new Star(400.f, -850.f));
        finishLineY = -1100.f;
    }
    else if (levelNum == 7) {
        //obstacles.push_back(new LaserBeam(400.f, 250.f, 300.f, 10.f, 0.f)); 
        stars.push_back(new Star(400.f, 150.f));
        obstacles.push_back(new ColorWheel(400.f, -50.f, 100.f, 18.f, 90.f));
        stars.push_back(new Star(400.f, -50.f));
        obstacles.push_back(new LaserBeam(200.f, -300.f, 100.f, 10.f, 150.f));
        stars.push_back(new Star(400.f, -350.f));
        obstacles.push_back(new ColorCross(400.f, -600.f, 180.f, 20.f, -80.f));
        stars.push_back(new Star(400.f, -600.f));
        obstacles.push_back(new Paddle(400.f, -900.f, 200.f, 25.f));
        obstacles.push_back(new LaserBeam(600.f, -1000.f, 100.f, 10.f, -200.f));
        stars.push_back(new Star(400.f, -1050.f));
        finishLineY = -1200.f;
    }
    else if (levelNum == 8) {
        obstacles.push_back(new ColorSquare(400.f, 200.f, 180.f, 20.f, 100.f));
        stars.push_back(new Star(400.f, 200.f));
        obstacles.push_back(new ColorSpotWheel(400.f, -150.f, 140.f, 15.f, 12, -60.f));
        stars.push_back(new Star(400.f, -150.f));
        obstacles.push_back(new LaserBeam(100.f, -400.f, 80.f, 10.f, 200.f));
        obstacles.push_back(new LaserBeam(700.f, -500.f, 80.f, 10.f, -200.f));
        stars.push_back(new Star(400.f, -450.f));
        obstacles.push_back(new ColorWheel(400.f, -800.f, 180.f, 18.f, 50.f));
        obstacles.push_back(new ColorCross(400.f, -800.f, 120.f, 15.f, -80.f));
        stars.push_back(new Star(400.f, -800.f));
        obstacles.push_back(new Paddle(400.f, -1100.f, 250.f, 25.f));
        obstacles.push_back(new LaserBeam(400.f, -1200.f, 400.f, 10.f, 0.f)); 
        stars.push_back(new Star(400.f, -1150.f));
        finishLineY = -1400.f;
    }
    else {
        finishLineY = -500.f;
    }
}

void Level::handleInput(sf::Event& event) {
    if (isGameOver || isWin || isDying) return;

    bool isTap = (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) ||
                 (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left);

    if (isTap) {
        if (waitingToStart) waitingToStart = false;
        player->jump();
    }
}

void Level::update(float dt, Game* game) {
    if (isGameOver || isWin) return;
    if (waitingToStart) return;

    // --- DEATH SEQUENCE LOGIC ---
    if (player->getHealth() <= 0 && !isDying) {
        isDying = true;
        deathTimer = 0.0f;
        shakeTimer = 0.5f; 
        
        player->explode(); // Trigger particles
        
        // Play dying sound once
        dyingSound.play(); 
    }

    if (isDying) {
        player->update(dt); // animate particles
        deathTimer += dt;
        if (shakeTimer > 0) shakeTimer -= dt;

        if (deathTimer >= 3.0f) {
            isGameOver = true; 
        }
        return; // Stop processing obstacles
    }
    // -----------------------------

    player->update(dt); 
    for (size_t i = 0; i < obstacles.size(); ++i) obstacles[i]->update(dt);
    for (size_t i = 0; i < stars.size(); ++i) stars[i]->update(dt);

    float healthPercent = player->getHealth() / 100.f;

    if (!waitingToStart) {
        if (healthPercent <= 0.1f && !potionSpawned && healthPotion == nullptr) {
            sf::Vector2f playerPos = player->getPosition();
            healthPotion = new HealthPotion(playerPos.x, playerPos.y - 100.f);
            potionSpawned = true;
        }
    }

    if (healthPotion != nullptr) {
        healthPotion->update(dt);
        if (healthPotion->isExpired()) {
            delete healthPotion;
            healthPotion = nullptr;
            potionSpawned = false;
        }
    }

    // --- COLLISION CHECKS ---
    sf::FloatRect pBounds = player->getBounds();
    sf::Vector2f pPos = player->getPosition();
    sf::Color pColor = player->getColor();

    for (size_t i = 0; i < obstacles.size(); ++i) {
        if (obstacles[i]->checkCollision(pBounds, pColor, pPos)) {
            float damage = obstacles[i]->getDamage();
            
            // Store health before hit
            float healthBeforeHit = player->getHealth();
            
            player->onCollision("Obstacle", damage, nullptr);
            player->move(0, 20.f); 
            
            // Audio: Play reduce sound only if health dropped AND is not 0 (not dead yet)
            // If health becomes 0, the 'whileDying' logic at top of next frame handles the sound.
            if (player->getHealth() < healthBeforeHit && player->getHealth() > 0.0f) {
                damageSound.play();
            }
        }
    }

    for (size_t i = 0; i < stars.size(); ++i) {
        if (!stars[i]->isCollected() && pBounds.intersects(stars[i]->getBounds())) {
            stars[i]->collect();
            player->onCollision("Star", 0.0f, game);
        }
    }

    if (healthPotion != nullptr && !healthPotion->hasBeenCollected()) {
        if (pBounds.intersects(healthPotion->getBounds())) {
            float healAmount = healthPotion->getHealAmount();
            float newHealth = player->getHealth() + healAmount;
            player->setHealth(newHealth);
            healthPotion->collect();
            potionSpawned = false;
        }
    }
    
    // Death by falling
    if (pPos.y > 630.f) { 
        player->setHealth(0.0f); // Will trigger isDying logic in next update
    }

    scoreText.setString(std::to_string(player->getScore()));

    if (pPos.y < finishLineY) isWin = true;

    // camera scroll
    if (pPos.y < 300.f) {
        float offset = 300.f - pPos.y;
        player->setPosition(pPos.x, 300.f);
        moveWorld(offset);
    }
}

void Level::moveWorld(float offset) {
    for (size_t i = 0; i < obstacles.size(); ++i) obstacles[i]->move(0, offset);
    for (size_t i = 0; i < stars.size(); ++i) stars[i]->move(0, offset);
    finishLineY += offset;

    float bgSpeed = offset * 0.5f; 
    bgSprite1.move(0, bgSpeed);
    bgSprite2.move(0, bgSpeed);

    float bgH = static_cast<float>(bgTexture.getSize().y);

    if (bgSprite1.getPosition().y > 800) {
        bgSprite1.setPosition(0, bgSprite2.getPosition().y - bgH);
    }
    if (bgSprite2.getPosition().y > 800) {
        bgSprite2.setPosition(0, bgSprite1.getPosition().y - bgH);
    }
}

void Level::draw() {
    sf::View originalView = window.getView();
    
    // Screen shake logic
    if (isDying && shakeTimer > 0) {
        float intensity = 10.0f; 
        float offX = (rand() % 100 / 50.f - 1.f) * intensity; 
        float offY = (rand() % 100 / 50.f - 1.f) * intensity;
        
        sf::View shakeView = originalView;
        shakeView.move(offX, offY);
        window.setView(shakeView);
    }

    window.draw(bgSprite1);
    window.draw(bgSprite2);

    for (size_t i = 0; i < obstacles.size(); ++i) obstacles[i]->Draw(window);
    for (size_t i = 0; i < stars.size(); ++i) stars[i]->Draw(window);

    if (healthPotion != nullptr && !healthPotion->hasBeenCollected()) {
        healthPotion->Draw(window);
    }
    
    if (!isGameOver && !isWin) {
        player->Draw(window);
        window.draw(scoreIcon);
        window.draw(scoreText);
    }

    // Restore view after shake
    window.setView(originalView);
}