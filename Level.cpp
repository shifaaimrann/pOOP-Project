// Level.cpp
#include "Header Files/Level.hpp"
#include <iostream>
#include "Header Files/game.hpp"
#include <cmath> // Added for collision math

// Level Constructor
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

    // Score setup - Moved to Top Right
    scoreText.setFont(font);
    scoreText.setCharacterSize(40);
    // Assuming window width is 800. Position at roughly x=700, y=38
    scoreText.setPosition(735, 20); 
    
    scoreIcon.setTexture(starTexture);
    scoreIcon.setScale(0.2f, 0.2f);
    // Position icon to the left of the text
    scoreIcon.setPosition(680, 20);

    // --- AUDIO SETUP ---
    if (!damageBuffer.loadFromFile("audio/healthbarreduce.mp3")) {
        std::cerr << "Error loading audio/healthbarreduce.mp3" << std::endl;
    }
    damageSound.setBuffer(damageBuffer);

    if (!dyingBuffer.loadFromFile("audio/whileDying.mp3")) {
        std::cerr << "Error loading audio/whileDying.mp3" << std::endl;
    }
    dyingSound.setBuffer(dyingBuffer);

    // --- WINNING SEQUENCE SETUP (Finish Line Texture) ---
    isWinning = false;
    winTimer = 0.f;
    
    // Create checkerboard pattern programmatically
    sf::Image checkImg;
    checkImg.create(800, 40, sf::Color::White);
    for(unsigned int x = 0; x < 800; ++x) {
        for(unsigned int y = 0; y < 40; ++y) {
            // 20x20 pixel checks
            if ( ((x / 20) + (y / 20)) % 2 == 1 ) {
                checkImg.setPixel(x, y, sf::Color::Black);
            }
        }
    }
    finishLineTexture.loadFromImage(checkImg);
    finishLineSprite.setTexture(finishLineTexture);
    finishLineSprite.setOrigin(0, 20); // Center origin Y

    // Reset Flags
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

    for (size_t i = 0; i < colorChangers.size(); ++i) delete colorChangers[i];
    colorChangers.clear();
}

// Helper to spawn confetti particles
void Level::spawnConfetti() {
    confetti.clear();
    // Spawn 150 particles
    for(int i = 0; i < 150; ++i) {
        Particle p;
        // Start above the screen
        p.position = sf::Vector2f(std::rand() % 800, -10.f - (std::rand() % 300));
        
        // Random fall speed and slight horizontal drift
        p.velocity = sf::Vector2f((std::rand() % 100 - 50), 100.f + (std::rand() % 200));
        
        p.lifetime = sf::seconds(4.0f); 
        
        p.shape.setSize(sf::Vector2f(8, 8));
        p.shape.setOrigin(4, 4);
        
        // Random bright colors
        int c = std::rand() % 5;
        if(c==0) p.color = sf::Color::Red;
        else if(c==1) p.color = sf::Color::Green;
        else if(c==2) p.color = sf::Color::Blue;
        else if(c==3) p.color = sf::Color::Yellow;
        else p.color = sf::Color::Magenta;
        
        p.shape.setFillColor(p.color);
        p.shape.setPosition(p.position);
        
        confetti.push_back(p);
    }
}
void Level::loadLevel(int levelNum, Game* g1) {
    currentLevel = levelNum;
    isGameOver = false;
    isWin = false;
    waitingToStart = true;
    potionSpawned = false;
    isDying = false;
    deathTimer = 0.f;
    shakeTimer = 0.f;
    isWinning = false;
    winTimer = 0.f;
    confetti.clear();

    for (auto* obs : obstacles) delete obs;
    obstacles.clear();
    for (auto* s : stars) delete s;
    stars.clear();
    for (auto* c : colorChangers) delete c;
    colorChangers.clear();

    if (healthPotion != nullptr) { delete healthPotion; healthPotion = nullptr; }
    if (player) delete player;
    player = new PlayerSprite(400.f, 500.f);

    std::string bgFile = "img/level backgrounds/level" + std::to_string(levelNum) + ".png";
    if (!bgTexture.loadFromFile(bgFile)) bgTexture.loadFromFile("img/level backgrounds/level1.png"); 
    
    bgSprite1.setTexture(bgTexture);
    bgSprite2.setTexture(bgTexture);
    bgSprite1.setPosition(0, 0);
    bgSprite2.setPosition(0, -static_cast<float>(bgTexture.getSize().y));

    // ==========================================
    //           LEVEL DESIGNS
    // ==========================================

    if (levelNum == 1) {
        // --- Intro: Simple Wheel & Paddle ---
        obstacles.push_back(new ColorWheel(400.f, 200.f, 100.f, 18.f, 90.f));
        stars.push_back(new Star(400.f, 200.f));

        colorChangers.push_back(new ColorChanger(400.f, 0.f)); 

        obstacles.push_back(new Paddle(400.f, -200.f, 200.f, 25.f));
        stars.push_back(new Star(400.f, -250.f)); 

        finishLineY = -400.f;
    }
    else if (levelNum == 2) {
        // --- Two Wheels rotating opposite directions ---
        obstacles.push_back(new ColorWheel(400.f, 150.f, 100.f, 18.f, 80.f));
        stars.push_back(new Star(400.f, 150.f));
        
        colorChangers.push_back(new ColorChanger(400.f, -50.f));

        obstacles.push_back(new ColorWheel(400.f, -250.f, 100.f, 18.f, -80.f));
        stars.push_back(new Star(400.f, -250.f));

        obstacles.push_back(new Paddle(400.f, -450.f, 200.f, 25.f));
        stars.push_back(new Star(400.f, -500.f));

        finishLineY = -650.f;
    }
    else if (levelNum == 3) {
        // --- Introduction to the Square ---
        obstacles.push_back(new ColorSquare(400.f, 150.f, 200.f, 20.f, 60.f));
        stars.push_back(new Star(400.f, 150.f));
        
        colorChangers.push_back(new ColorChanger(400.f, -50.f));

        obstacles.push_back(new ColorWheel(400.f, -300.f, 100.f, 18.f, 90.f));
        stars.push_back(new Star(400.f, -300.f));

        colorChangers.push_back(new ColorChanger(400.f, -450.f));

        obstacles.push_back(new ColorSquare(400.f, -650.f, 200.f, 20.f, -70.f));
        stars.push_back(new Star(400.f, -650.f));

        finishLineY = -850.f;
    }
    else if (levelNum == 4) {
        // --- The "Color Cross" (Fixed at x=350) ---
        obstacles.push_back(new ColorCross(350.f, 200.f, 180.f, 20.f, 50.f)); 
        stars.push_back(new Star(350.f, 200.f));
        
        colorChangers.push_back(new ColorChanger(400.f, 0.f));

        obstacles.push_back(new Paddle(250.f, -150.f, 150.f, 25.f)); // Left paddle
        obstacles.push_back(new Paddle(550.f, -150.f, 150.f, 25.f)); // Right paddle
        stars.push_back(new Star(400.f, -150.f)); // Star in gap

        colorChangers.push_back(new ColorChanger(400.f, -300.f));

        // Another cross, faster
        obstacles.push_back(new ColorCross(350.f, -500.f, 180.f, 20.f, -70.f));
        stars.push_back(new Star(350.f, -500.f));

        finishLineY = -700.f;
    }
    else if (levelNum == 5) {
        // --- Nested: Small Wheel inside Big Square ---
        // Big Square
        obstacles.push_back(new ColorSquare(400.f, 100.f, 280.f, 20.f, 40.f));
        // Small Wheel inside
        obstacles.push_back(new ColorWheel(400.f, 100.f, 90.f, 15.f, -60.f));
        stars.push_back(new Star(400.f, 100.f));

        colorChangers.push_back(new ColorChanger(400.f, -150.f));

        obstacles.push_back(new ColorSpotWheel(400.f, -350.f, 130.f, 15.f, 12, 50.f));
        stars.push_back(new Star(400.f, -350.f));

        finishLineY = -550.f;
    }
    else if (levelNum == 6) {
        // --- Nested: Cross inside Wheel ---
        // Big Wheel
        obstacles.push_back(new ColorWheel(400.f, 150.f, 160.f, 18.f, 50.f));
        // Cross inside (Fixed at x=350 is tricky inside a 400 wheel, so we move wheel to 350)
        obstacles.push_back(new ColorWheel(350.f, -100.f, 160.f, 18.f, -50.f));
        obstacles.push_back(new ColorCross(350.f, -100.f, 120.f, 18.f, -50.f));
        stars.push_back(new Star(350.f, -100.f));

        colorChangers.push_back(new ColorChanger(350.f, -300.f));

        obstacles.push_back(new ColorSquare(350.f, -500.f, 200.f, 20.f, 80.f));
        stars.push_back(new Star(350.f, -500.f));

        finishLineY = -700.f;
    }
    else if (levelNum == 7) {
        // --- Lasers & Speed ---
        // Laser guarding a star
        obstacles.push_back(new LaserBeam(400.f, 200.f, 200.f, 10.f, 100.f)); // Moving laser
        stars.push_back(new Star(400.f, 100.f));

        colorChangers.push_back(new ColorChanger(400.f, -50.f));

        obstacles.push_back(new ColorCross(350.f, -250.f, 180.f, 20.f, 90.f));
        stars.push_back(new Star(350.f, -250.f));

        // Double Laser Gate
        obstacles.push_back(new LaserBeam(200.f, -450.f, 150.f, 10.f, 0.f));
        obstacles.push_back(new LaserBeam(600.f, -450.f, 150.f, 10.f, 0.f));
        stars.push_back(new Star(400.f, -450.f)); // Safe in middle

        colorChangers.push_back(new ColorChanger(400.f, -600.f));

        obstacles.push_back(new ColorSpotWheel(400.f, -800.f, 140.f, 18.f, 16, -60.f));
        stars.push_back(new Star(400.f, -800.f));

        finishLineY = -1000.f;
    }
    else if (levelNum == 8) {
        // --- THE GAUNTLET (Boss Level) ---
        // 1. Nested Wheel in Square
        obstacles.push_back(new ColorSquare(400.f, 200.f, 260.f, 20.f, 50.f));
        obstacles.push_back(new ColorWheel(400.f, 200.f, 100.f, 15.f, -80.f));
        stars.push_back(new Star(400.f, 200.f));

        colorChangers.push_back(new ColorChanger(400.f, -50.f));

        // 2. Cross Field (x=350)
        obstacles.push_back(new ColorCross(350.f, -250.f, 180.f, 20.f, 100.f)); // Fast!
        stars.push_back(new Star(350.f, -250.f));

        colorChangers.push_back(new ColorChanger(350.f, -450.f));

        // 3. Laser Mayhem + Paddle
        obstacles.push_back(new LaserBeam(400.f, -600.f, 300.f, 10.f, 150.f)); // Moving fast
        obstacles.push_back(new Paddle(400.f, -750.f, 250.f, 25.f));
        stars.push_back(new Star(400.f, -700.f));

        // 4. Final Nested: Small Square inside Big Wheel
        obstacles.push_back(new ColorWheel(400.f, -1000.f, 180.f, 20.f, 60.f));
        obstacles.push_back(new ColorSquare(400.f, -1000.f, 150.f, 15.f, -60.f));
        stars.push_back(new Star(400.f, -1000.f));

        finishLineY = -1200.f;
    }
    else {
        finishLineY = -500.f; // Fallback
    }
    
    finishLineSprite.setPosition(0, finishLineY);
}

void Level::handleInput(sf::Event& event) {
    if (isGameOver || isWin || isDying || isWinning) return;

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

    // --- WINNING SEQUENCE ---
    if (player->getPosition().y <= finishLineY && !isWinning && !isDying) {
        isWinning = true;
        winTimer = 0.0f;
        spawnConfetti();
    }

    if (isWinning) {
        for(auto& p : confetti) {
            p.velocity.y += 200.f * dt; // Gravity
            p.position += p.velocity * dt;
            p.shape.setPosition(p.position);
            p.shape.rotate(100.f * dt); 
        }
        winTimer += dt;
        if (winTimer >= 3.0f) {
            isWin = true;
        }
        return; // Stop physics
    }

    // --- DEATH SEQUENCE ---
    if (player->getHealth() <= 0 && !isDying) {
        isDying = true;
        deathTimer = 0.0f;
        shakeTimer = 0.5f; 
        player->explode(); 
        dyingSound.play(); 
    }

    if (isDying) {
        player->update(dt); // particles
        deathTimer += dt;
        if (shakeTimer > 0) shakeTimer -= dt;

        if (deathTimer >= 3.0f) {
            isGameOver = true; 
        }
        return; // Stop physics
    }
    // -----------------------------

    // Normal Update
    player->update(dt); 
    for (size_t i = 0; i < obstacles.size(); ++i) obstacles[i]->update(dt);
    for (size_t i = 0; i < stars.size(); ++i) stars[i]->update(dt);
    for (size_t i = 0; i < colorChangers.size(); ++i) colorChangers[i]->update(dt); // New!

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

    // COLLISION
    // Re-get bounds after player update, just to be safe
    sf::FloatRect pBounds = player->getBounds();
    sf::Vector2f pPos = player->getPosition();
    sf::Color pColor = player->getColor();

    // Obstacles
    for (size_t i = 0; i < obstacles.size(); ++i) {
        if (obstacles[i]->checkCollision(pBounds, pColor, pPos)) {
            float damage = obstacles[i]->getDamage();
            float healthBefore = player->getHealth();

            player->onCollision("Obstacle", damage, nullptr);
            player->move(0, 20.f); 
            
            if (player->getHealth() < healthBefore && player->getHealth() > 0.0f) {
                if (damageSound.getStatus() != sf::Sound::Playing) {
                    damageSound.play();
                }
            }
        }
    }

    // Stars
    for (size_t i = 0; i < stars.size(); ++i) {
        if (!stars[i]->isCollected() && pBounds.intersects(stars[i]->getBounds())) {
            stars[i]->collect();
            player->onCollision("Star", 0.0f, game);
        }
    }

    // Color Changers (IMPROVED COLLISION LOGIC)
    // Refresh position in case obstacles moved player
    sf::Vector2f pCenter = player->getPosition(); // Center is position for PlayerSprite
    float pRadius = pBounds.width * 0.4f; // Approximation of hit circle (slightly smaller than box)

    for (size_t i = 0; i < colorChangers.size(); ++i) {
        if (colorChangers[i]->isActiveItem()) {
            
            // Use distance based collision for better feel with circles
            sf::FloatRect cBounds = colorChangers[i]->getBounds();
            sf::Vector2f cCenter = colorChangers[i]->getPosition(); 
            float cRadius = cBounds.width / 2.0f;

            float dx = pCenter.x - cCenter.x;
            float dy = pCenter.y - cCenter.y;
            float distance = std::sqrt(dx*dx + dy*dy);

            // If distance < sum of radii, we hit it
            if (distance < (pRadius + cRadius)) {
                colorChangers[i]->collect();
                player->onCollision("ColorChanger", 0.0f, game);
            }
        }
    }

    // Potion
    if (healthPotion != nullptr && !healthPotion->hasBeenCollected()) {
        if (pBounds.intersects(healthPotion->getBounds())) {
            player->setHealth(player->getHealth() + healthPotion->getHealAmount());
            healthPotion->collect();
            potionSpawned = false;
        }
    }
    
    // Fall death
    if (pPos.y > 630.f) { 
        player->setHealth(0.0f); 
    }

    scoreText.setString(std::to_string(player->getScore()));

    // Camera scroll
    if (pPos.y < 300.f) {
        float offset = 300.f - pPos.y;
        player->setPosition(pPos.x, 300.f);
        moveWorld(offset);
    }
}

void Level::moveWorld(float offset) {
    for (size_t i = 0; i < obstacles.size(); ++i) obstacles[i]->move(0, offset);
    for (size_t i = 0; i < stars.size(); ++i) stars[i]->move(0, offset);
    for (size_t i = 0; i < colorChangers.size(); ++i) colorChangers[i]->move(0, offset);

    finishLineY += offset;
    finishLineSprite.move(0, offset);

    float bgSpeed = offset * 0.5f; 
    bgSprite1.move(0, bgSpeed);
    bgSprite2.move(0, bgSpeed);

    float bgH = static_cast<float>(bgTexture.getSize().y);
    if (bgSprite1.getPosition().y > 800) bgSprite1.setPosition(0, bgSprite2.getPosition().y - bgH);
    if (bgSprite2.getPosition().y > 800) bgSprite2.setPosition(0, bgSprite1.getPosition().y - bgH);
}

void Level::draw() {
    sf::View originalView = window.getView();
    
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
    
    // Finish line
    window.draw(finishLineSprite);

    for (size_t i = 0; i < obstacles.size(); ++i) obstacles[i]->Draw(window);
    for (size_t i = 0; i < stars.size(); ++i) stars[i]->Draw(window);
    for (size_t i = 0; i < colorChangers.size(); ++i) colorChangers[i]->Draw(window);

    if (healthPotion != nullptr && !healthPotion->hasBeenCollected()) {
        healthPotion->Draw(window);
    }
    
    if (!isGameOver && !isWin) {
        player->Draw(window);
        window.draw(scoreIcon);
        window.draw(scoreText);
    }

    if (isWinning) {
        for (const auto& p : confetti) {
            window.draw(p.shape);
        }
    }

    window.setView(originalView);
}