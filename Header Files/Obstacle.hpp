#pragma once
#include "Element.hpp"

#include <vector>
class Obstacle: public Element{
    public:
        void Draw();
        void update();
        void updateHealth();
    protected:
        std::vector <double> size;
};