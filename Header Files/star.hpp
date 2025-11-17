#pragma once
#include "Element.hpp"

class Star : public Element {
public:
    Star(float x, float y); // constructor
    virtual ~Star() {}

    // from Element base class
    virtual void update(float dt) override;
    virtual void Draw(sf::RenderWindow& window) override;

    // star-specific functions
    bool isCollected() const;
    void collect();
    sf::FloatRect getBounds() const; // for collision

private:
    sf::Texture starTexture;
    sf::Sprite starSprite;
    bool collected;
};