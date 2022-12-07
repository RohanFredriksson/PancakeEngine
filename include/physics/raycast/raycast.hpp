#pragma once

#include <glm/glm.hpp>
#include "physics/primitives/rigidbody.hpp"

using glm::vec2;

class Ray {
  
    public:

        vec2 origin;
        vec2 direction;

        Ray(vec2 origin, vec2 direction);

};

class RaycastResult {

    public:

        vec2 point;
        vec2 normal;
        float distance;
        Entity* hit;

        RaycastResult(vec2 point, vec2 normal, float distance, Entity* hit);
        RaycastResult();
        
};

namespace Raycast {
    RaycastResult raycast(Rigidbody* rigidbody, Ray* ray);
}
