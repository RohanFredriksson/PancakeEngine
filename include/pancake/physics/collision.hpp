#pragma once

#include "pancake/physics/collider.hpp"

#include "glm/glm.hpp"
using glm::vec2;

namespace Pancake {

    class CollisionManifold {

        public:

            vec2 normal;
            vec2 contactPoint;
            float depth;
            bool colliding;

            CollisionManifold();
            CollisionManifold(vec2 normal, vec2 contactPoint, float depth);

    };

    namespace Collision {
        CollisionManifold findCollisionFeatures(Collider* c1, Collider* c2);
    }

}