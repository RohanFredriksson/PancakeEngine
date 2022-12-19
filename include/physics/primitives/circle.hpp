#pragma once

#include "physics/primitives/collider.hpp"

class Circle : public Collider {

    private:

        float radius;

    public:

        Circle();

        float getRadius();
        Circle* setRadius(float radius);
};