#pragma once

#include <glm/glm.hpp>
#include "pancake/physics/collider.hpp"

using glm::vec2;

class Box : public Collider {

    private:

        vec2 size;
        vec2 halfSize;

    public:

        Box();
        json serialise() override;
        bool load(json j) override;

        float getMomentOfInertia() override;
        vec2 getSize();
        vec2 getHalfSize();
        vec2 getMin();
        vec2 getMax();

        Box* setSize(vec2 size);

};