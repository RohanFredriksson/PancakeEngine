#pragma once

#include "pancake/physics/collisionmanifold.hpp"
#include "pancake/physics/collider.hpp"

namespace Pancake {

    namespace Collision {
        CollisionManifold* findCollisionFeatures(Collider* c1, Collider* c2);
    }

}