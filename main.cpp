// main.cpp
#include "Header Files/game.hpp"


int main() {
    // Game game;
    // game.run();
    
    // //Game* Game::instancepointer=nullptr;
    Game::getInstance().run();
    //game.run();
    //std::cout << " total score: " << game.getScore() << std::endl;
    return 0;
}