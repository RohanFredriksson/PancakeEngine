#pragma once

#include <glm/glm.hpp>
#include "physics/primitives/collider.hpp"

using glm::vec2;

class Box : public Collider {

    private:

        vec2 size;
        vec2 halfSize;

    public:

        Box();

        vec2 getSize();
        vec2 getHalfSize();
        vec2 getMin();
        vec2 getMax();

        Box* setSize(vec2 size);

};