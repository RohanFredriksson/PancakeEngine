#pragma once

#include "pancake/physics/primitives/collider.hpp"

class Circle : public Collider {

    private:

        float radius;

    public:

        static Collider* create();
        Circle();
        json serialise() override;
        bool load(json j) override;

        float getMomentOfInertia() override;
        float getRadius();
        Circle* setRadius(float radius);
};