#pragma once

#include "pancake/physics/primitives/collider.hpp"

class Circle : public Collider {

    private:

        float radius;

    public:

        Circle();
        json serialise() override;

        float getMomentOfInertia() override;
        float getRadius();
        Circle* setRadius(float radius);
};