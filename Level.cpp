#include "Header Files/Level.hpp"
#include <iostream>

Level::Level(sf::RenderWindow& win) : window(win) {
    // --- 1. setup background ---
    const sf::Color lightOrange(255, 178, 102);
    const sf::Color darkOrange(255, 140, 51);
    const sf::Color borderBgColor(75, 0, 130);
    const sf::Color segmentColor(55, 0, 90, 200);
    const sf::Color lineHighlight(138, 43, 226);
    const float borderWidth = 48.f;
    const float stripeHeight = 60.f;
    const float segmentHeight = 60.f;
    const int numStripes = 14; // increased slightly to cover screen fully
    const int numSegments = 14;

    // setup stripes
    for (int i = 0; i < numStripes; ++i) {
        sf::RectangleShape s(sf::Vector2f(800, stripeHeight));
        s.setFillColor((i % 2 == 0) ? lightOrange : darkOrange);
        s.setPosition(0, i * stripeHeight);
        stripes.push_back(s);
    }

    // setup borders
    leftBorderBg.setSize(sf::Vector2f(borderWidth, 600));
    leftBorderBg.setFillColor(borderBgColor);
    leftBorderBg.setPosition(0, 0);

    rightBorderBg.setSize(sf::Vector2f(borderWidth, 600));
    rightBorderBg.setFillColor(borderBgColor);
    rightBorderBg.setPosition(800 - borderWidth, 0);

    leftLine.setSize(sf::Vector2f(2, 600));
    leftLine.setFillColor(lineHighlight);
    leftLine.setPosition(borderWidth, 0);

    rightLine.setSize(sf::Vector2f(2, 600));
    rightLine.setFillColor(lineHighlight);
    rightLine.setPosition(800 - borderWidth - 2, 0);

    // setup border segments
    for (int i = 0; i < numSegments; ++i) {
        sf::RectangleShape l(sf::Vector2f(borderWidth - 8, segmentHeight - 8));
        l.setFillColor(segmentColor);
        l.setPosition(4, (i * segmentHeight) + 4);
        leftSegments.push_back(l);

        sf::RectangleShape r(sf::Vector2f(borderWidth - 8, segmentHeight - 8));
        r.setFillColor(segmentColor);
        r.setPosition((800 - borderWidth) + 4, (i * segmentHeight) + 4);
        rightSegments.push_back(r);
    }

    // --- 2. setup game objects ---
    player = new PlayerSprite(400.f, 500.f);
    healthBar = new HealthBar(100.f, { 10.f, 10.f }, { 200.f, 20.f });
    
    font.loadFromFile("fonts/font.ttf");
    starTexture.loadFromFile("img/star.png");

    // --- 3. setup ui ---
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Red);
    sf::FloatRect gr = gameOverText.getLocalBounds();
    gameOverText.setOrigin(gr.width/2, gr.height/2);
    gameOverText.setPosition(400, 300);

    winText.setFont(font);
    winText.setString("YOU WIN!");
    winText.setCharacterSize(50);
    winText.setFillColor(sf::Color::Green);
    sf::FloatRect wr = winText.getLocalBounds();
    winText.setOrigin(wr.width/2, wr.height/2);
    winText.setPosition(400, 250);

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
    delete player;
    delete healthBar;
    for (auto* o : obstacles) delete o;
    for (auto* s : stars) delete s;
}

void Level::loadLevel(int levelNum) {
    currentLevel = levelNum;
    isGameOver = false;
    isWin = false;
    
    for (auto* o : obstacles) delete o;
    obstacles.clear();
    for (auto* s : stars) delete s;
    stars.clear();

    delete player;
    player = new PlayerSprite(400.f, 500.f);
    healthBar->setHealth(100.f); 
    
    // --- level layout ---
    obstacles.push_back(new ColorWheel(400.f, 200.f, 100.f, 18.f, 90.f));
    stars.push_back(new Star(400.f, 200.f));

    obstacles.push_back(new Paddle(400.f, -150.f, 200.f, 25.f));
    stars.push_back(new Star(400.f, -300.f)); 

    obstacles.push_back(new ColorWheel(400.f, -550.f, 100.f, 18.f, -100.f));
    stars.push_back(new Star(400.f, -550.f));

    finishLineY = -700.f;
}

void Level::handleInput(sf::Event& event) {
    if (isGameOver || isWin) {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
            loadLevel(currentLevel);
        }
        return;
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            player->jump();
        }
        if (event.key.code == sf::Keyboard::C) {
            static int cIdx = 0;
            sf::Color cols[] = {sf::Color::Magenta, sf::Color::Blue, sf::Color::Yellow, sf::Color::Red};
            cIdx = (cIdx + 1) % 4;
            player->changeColor(cols[cIdx]);
        }
    }
}

void Level::update(float dt) {
    if (isGameOver || isWin) return;

    player->update(dt); 
    for (auto* o : obstacles) o->update(dt);
    for (auto* s : stars) s->update(dt);

    // background auto-scroll (parallax)
    float bgSpeed = 80.f;
    for (auto& s : stripes) {
        s.move(0, bgSpeed * dt);
        if (s.getPosition().y > 800) s.move(0, -14 * 60.f); // wrap around
    }
    for (auto& l : leftSegments) {
        l.move(0, bgSpeed * dt);
        if (l.getPosition().y > 800) l.move(0, -12 * 60.f);
    }
    for (auto& r : rightSegments) {
        r.move(0, bgSpeed * dt);
        if (r.getPosition().y > 800) r.move(0, -12 * 60.f);
    }

    // collision detection
    sf::FloatRect pBounds = player->getBounds();
    sf::Vector2f pPos = player->getPosition();
    sf::Color pColor = player->getColor();

    for (auto* obs : obstacles) {
        if (obs->checkCollision(pBounds, pColor, pPos)) {
            player->onCollision("Obstacle");
            healthBar->damage(30.f); 
            player->move(0, 20.f); 
        }
    }

    for (auto* s : stars) {
        if (!s->isCollected() && pBounds.intersects(s->getBounds())) {
            s->collect();
            player->onCollision("Star");
            healthBar->heal(20.f);
        }
    }

    healthBar->setHealth(player->getHealth());
    scoreText.setString(std::to_string(player->getScore()));

    if (player->getHealth() <= 0) {
        isGameOver = true;
    }
    if (pPos.y < finishLineY) {
        isWin = true;
        winText.setString("YOU WIN!\nScore: " + std::to_string(player->getScore()));
        sf::FloatRect wr = winText.getLocalBounds();
        winText.setOrigin(wr.width/2, wr.height/2);
        winText.setPosition(400, 300);
    }

    // camera scrolling (move world down when player goes up)
    if (pPos.y < 300.f) {
        float offset = 300.f - pPos.y;
        player->setPosition(pPos.x, 300.f);
        moveWorld(offset);
    }
}

void Level::moveWorld(float offset) {
    for (auto* o : obstacles) o->move(0, offset);
    for (auto* s : stars) s->move(0, offset);
    
    // move background faster for parallax effect? 
    // or just same speed to keep it simple. let's do same speed.
    for (auto& s : stripes) {
        s.move(0, offset);
        if (s.getPosition().y > 800) s.move(0, -14 * 60.f);
    }
    // (technically segments should wrap too, but simplicity first)
    
    finishLineY += offset;
}

void Level::draw() {
    // 1. draw background first
    for (const auto& s : stripes) window.draw(s);
    window.draw(leftBorderBg);
    window.draw(rightBorderBg);
    for (const auto& l : leftSegments) window.draw(l);
    for (const auto& r : rightSegments) window.draw(r);
    window.draw(leftLine);
    window.draw(rightLine);

    // 2. draw game objects
    for (auto* o : obstacles) o->Draw(window);
    for (auto* s : stars) s->Draw(window);
    
    // 3. draw ui / player
    if (!isGameOver && !isWin) {
        player->Draw(window);
        healthBar->Draw(window);
        window.draw(scoreIcon);
        window.draw(scoreText);
    }
    else if (isGameOver) {
        window.draw(gameOverText);
    }
    else if (isWin) {
        window.draw(winText);
    }
}