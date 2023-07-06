#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "pancake/physics/collider.hpp"

using glm::vec2;

namespace Pancake {

    class CollisionManifold {

        public:

            vec2 normal;
            vec2 point;
            float depth;

            CollisionManifold();
            CollisionManifold(vec2 normal, vec2 point, float depth);
            CollisionManifold flip();

    };

    namespace Collision {
        std::vector<CollisionManifold> findCollisionFeatures(Collider* c1, Collider* c2);
    }

}