#pragma once

#include "pancake/physics/collider.hpp"

class Circle : public Collider {

    private:

        float radius;

    public:

        Circle();
        json serialise() override;
        bool load(json j) override;

        float getMomentOfInertia() override;
        float getRadius();
        Circle* setRadius(float radius);
};