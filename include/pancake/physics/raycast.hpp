#pragma once

#include <glm/glm.hpp>
#include "pancake/physics/rigidbody.hpp"

using glm::vec2;

namespace Pancake {

    class Ray {
    
        public:

            vec2 origin;
            vec2 direction;

            Ray(vec2 origin, vec2 direction);
            void rotate(vec2 around, float rCos, float rSin);

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
        RaycastResult raycast(Rigidbody* rigidbody, Ray ray);
    }

}