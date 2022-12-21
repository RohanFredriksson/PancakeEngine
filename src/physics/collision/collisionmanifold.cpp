#include "physics/collision/collisionmanifold.hpp"

CollisionManifold::CollisionManifold() {
    this->normal = glm::vec2(0.0f, 0.0f);
    this->contactPoint = glm::vec2(0.0f, 0.0f);
    this->depth = 0.0f;
}

CollisionManifold::CollisionManifold(vec2 normal, vec2 contactPoint, float depth) {
    this->normal = normal;
    this->contactPoint = contactPoint;
    this->depth = depth;
}