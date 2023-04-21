#pragma once

#include "glm/glm.hpp"

using glm::vec2;

namespace Pancake {

    class CollisionManifold {

        public:

            vec2 normal;
            vec2 contactPoint;
            float depth;

            CollisionManifold();
            CollisionManifold(vec2 normal, vec2 contactPoint, float depth);

    };

}