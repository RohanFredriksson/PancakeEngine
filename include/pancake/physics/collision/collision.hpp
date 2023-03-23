#pragma once

#include "pancake/physics/collision/collisionmanifold.hpp"
#include "pancake/physics/primitives/collider.hpp"

namespace Collision {
    CollisionManifold* findCollisionFeatures(Collider* c1, Collider* c2);
}