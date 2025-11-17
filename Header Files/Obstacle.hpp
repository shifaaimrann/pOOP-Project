#pragma once
#include "Element.hpp"

// forward declaration
// this tells the compiler "trust me, a class called PlayerSprite exists"
// this avoids a circular include loop.
class PlayerSprite; 

// an abstract class for all obstacles, inheriting from element
// it provides a clear "obstacle" type for collision
// and can add functions that all obstacles must have.
class Obstacle : public Element {
public:
    // we need a virtual destructor for any base class
    virtual ~Obstacle() {}

    // all obstacles must have a way to report their collision
    // bounds, so we make it a pure virtual function.
    virtual sf::FloatRect getBounds() const = 0;

    // a function for player-obstacle interaction
    // this now works because of the forward declaration above
    virtual void onCollision(PlayerSprite& player) {
        // default implementation does nothing
    }

protected:
    // protected constructor so only child classes can call it
    Obstacle() {}
};