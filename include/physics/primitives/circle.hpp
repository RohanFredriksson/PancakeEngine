#pragma once

#include "physics/primitives/collider.hpp"

class Circle : public Collider {

    private:

        float radius;

    public:

        Circle(float radius);
        ~Circle();

        float getRadius();
        void setRadius(float radius);
};