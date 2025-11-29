// Level.cpp
#include "Header Files/Level.hpp"
#include <iostream>
#include "Header Files/game.hpp"

Level::Level(sf::RenderWindow& win) : window(win) , healthPotion(nullptr){
    player = nullptr;
    potionSpawned = false;

    font.loadFromFile("fonts/font.ttf");
    starTexture.loadFromFile("img/star.png");

    // score setup
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setPosition(55, 38);
    
    scoreIcon.setTexture(starTexture);
    scoreIcon.setScale(0.08f, 0.08f);
    scoreIcon.setPosition(15, 40);

    isGameOver = false;
    isWin = false;
    currentLevel = 1;
}

Level::~Level() {
    // clean up single objects
    if (player) delete player;
    if (healthPotion != nullptr) {
        delete healthPotion;
    }

    // clean up vectors of pointers
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
    // clear previous level data
    for (size_t i = 0; i < obstacles.size(); ++i) delete obstacles[i];
    obstacles.clear();
    for (size_t i = 0; i < stars.size(); ++i) delete stars[i];
    stars.clear();

        //Clean up old potion if it exists
    if (healthPotion != nullptr) {
        delete healthPotion;
        healthPotion = nullptr;
    }
    
  if (player) delete player;
    player = new PlayerSprite(400.f, 500.f);

    // --- load infinite background ---
    std::string bgFile = "img/level backgrounds/level" + std::to_string(levelNum) + ".png";
    if (!bgTexture.loadFromFile(bgFile)) {
        bgTexture.loadFromFile("img/level backgrounds/level1.png"); // fallback
    }
    
    bgSprite1.setTexture(bgTexture);
    bgSprite2.setTexture(bgTexture);
    bgSprite1.setPosition(0, 0);
    bgSprite2.setPosition(0, -static_cast<float>(bgTexture.getSize().y));

    // --- LEVEL LAYOUTS ---
    
    // LEVEL 1: Wheel -> Paddle -> Wheel (3 Stars)
    if (levelNum == 1) {
        // 1. Wheel + Star
        obstacles.push_back(new ColorWheel(400.f, 200.f, 100.f, 18.f, 90.f));
        stars.push_back(new Star(400.f, 200.f));

        // 2. Paddle + Star
        obstacles.push_back(new Paddle(400.f, -100.f, 200.f, 25.f));
        stars.push_back(new Star(400.f, -150.f)); 

        // 3. Wheel + Star
        obstacles.push_back(new ColorWheel(400.f, -400.f, 100.f, 18.f, -90.f));
        stars.push_back(new Star(400.f, -400.f));

        finishLineY = -600.f;
    }
    // LEVEL 2: Wheel -> Paddle -> Paddle -> Wheel -> Paddle (5 Stars)
    else if (levelNum == 2) {
        // 1. Wheel + Star
        obstacles.push_back(new ColorWheel(400.f, 200.f, 100.f, 18.f, 80.f));
        stars.push_back(new Star(400.f, 200.f));

        // 2. Paddle + Star
        obstacles.push_back(new Paddle(400.f, -100.f, 200.f, 25.f));
        stars.push_back(new Star(400.f, -130.f));

        // 3. Paddle (Faster) + Star
        obstacles.push_back(new Paddle(400.f, -400.f, 200.f, 25.f)); // You might want to increase speed in Paddle class later
        stars.push_back(new Star(400.f, -430.f));

        // 4. Wheel (Counter-clockwise) + Star
        obstacles.push_back(new ColorWheel(400.f, -700.f, 100.f, 18.f, -100.f));
        stars.push_back(new Star(400.f, -700.f));

        // 5. Paddle + Star
        obstacles.push_back(new Paddle(400.f, -1000.f, 200.f, 25.f));
        stars.push_back(new Star(400.f, -1030.f));

        finishLineY = -1200.f;
    }
    // LEVEL 3: Wheel-in-Wheel -> Paddle-in-Wheel -> Paddle -> Wheel (5 Stars)
    else if (levelNum == 3) {
        // 1. Wheel inside a Wheel (Concentric)
        // Outer Wheel (Larger Radius: 160)
        obstacles.push_back(new ColorWheel(400.f, 200.f, 200.f, 18.f, 60.f));
        // Inner Wheel (Smaller Radius: 100) - Rotates opposite direction (-60)
        obstacles.push_back(new ColorWheel(400.f, 200.f, 100.f, 18.f, -50.f));
        stars.push_back(new Star(400.f, 200.f)); // Star in the very center

        // 2. Paddle inside a Bigger Wheel
        // Big Wheel (Radius 180)
        obstacles.push_back(new ColorWheel(400.f, -300.f, 180.f, 18.f, 50.f));
        // Paddle in the center of it
        obstacles.push_back(new Paddle(400.f, -300.f, 180.f, 25.f));
        stars.push_back(new Star(400.f, -200.f)); 
        stars.push_back(new Star(400.f, -60.f)); // Star between obj 1 and 2

        // 3. Paddle + Star
        obstacles.push_back(new Paddle(400.f, -700.f, 200.f, 25.f));
        stars.push_back(new Star(400.f, -530.f));

        // 4. Wheel + Star
        obstacles.push_back(new ColorWheel(400.f, -900.f, 100.f, 18.f, 90.f));
        stars.push_back(new Star(400.f, -800.f));

        finishLineY = -1100.f;
    }
    else {
        // Default for Level 4-8 (Empty for now)
        finishLineY = -500.f;
    }
}

void Level::handleInput(sf::Event& event) {
    if (isGameOver || isWin) return;

    // Support both Spacebar AND Mouse Click for "Tap"
    bool isTap = (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) ||
                 (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left);

    if (isTap) {
        if (waitingToStart) {
            waitingToStart = false;
        }
        
        // Perform the jump
        player->jump();
    }
}

void Level::update(float dt, Game* game) {

    if (isGameOver || isWin) return;
    if (waitingToStart) {
        return; 
    }
    
    player->update(dt); 
    for (size_t i = 0; i < obstacles.size(); ++i) obstacles[i]->update(dt);
    for (size_t i = 0; i < stars.size(); ++i) stars[i]->update(dt);

    float healthPercent = player->getHealth() / 100.f;

// Don't spawn potion until player actually begins the level
if (!waitingToStart) {
    if (healthPercent <= 0.1f && !potionSpawned && healthPotion == nullptr) {
        sf::Vector2f playerPos = player->getPosition();
        healthPotion = new HealthPotion(playerPos.x, playerPos.y - 100.f);
        potionSpawned = true;
    }
}

    if (healthPotion != nullptr) {
        healthPotion->update(dt);
        
        // Check if expired (use the public method)
        if (healthPotion->isExpired()) {
            delete healthPotion;
            healthPotion = nullptr;
            potionSpawned = false;
        }
    }


    // collision checks
    sf::FloatRect pBounds = player->getBounds();
    sf::Vector2f pPos = player->getPosition();
    sf::Color pColor = player->getColor();

    for (size_t i = 0; i < obstacles.size(); ++i) {
        if (obstacles[i]->checkCollision(pBounds, pColor, pPos)) {
            float damage = obstacles[i]->getDamage();
            player->onCollision("Obstacle", damage, nullptr);
            player->move(0, 20.f); 
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
    
    // Screen height is 600. If player goes below 600 (plus a little buffer), they die.
    if (pPos.y > 630.f) { 
        isGameOver = true;
        player->setHealth(0.0f);
    }

    scoreText.setString(std::to_string(player->getScore()));

    if (player->getHealth() <= 0) isGameOver = true;
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

    // --- infinite background scrolling ---
    float bgSpeed = offset * 0.5f; 
    bgSprite1.move(0, bgSpeed);
    bgSprite2.move(0, bgSpeed);

    float bgH = static_cast<float>(bgTexture.getSize().y);

    // infinite loop logic
    if (bgSprite1.getPosition().y > 800) {
        bgSprite1.setPosition(0, bgSprite2.getPosition().y - bgH);
    }
    if (bgSprite2.getPosition().y > 800) {
        bgSprite2.setPosition(0, bgSprite1.getPosition().y - bgH);
    }
}

void Level::draw() {
    // draw background first
    window.draw(bgSprite1);
    window.draw(bgSprite2);

    // draw game objects
    for (size_t i = 0; i < obstacles.size(); ++i) obstacles[i]->Draw(window);
    for (size_t i = 0; i < stars.size(); ++i) stars[i]->Draw(window);


    if (healthPotion != nullptr && !healthPotion->hasBeenCollected()) {
        healthPotion->Draw(window);
}
    
    // draw ui
    if (!isGameOver && !isWin) {
        player->Draw(window);
        window.draw(scoreIcon);
        window.draw(scoreText);
    }
}