#pragma once

#include "physics/collision/collisionmanifold.hpp"
#include "physics/primitives/collider.hpp"

namespace Collision {
    CollisionManifold* findCollisionFeatures(Collider* c1, Collider* c2);
}