#pragma once

#include <glm/glm.hpp>
#include "pancake/physics/forcegenerator.hpp"

using glm::vec2;

namespace Pancake {

    class Gravity : public ForceGenerator {

        private:

            vec2 gravity;

        public:

            Gravity(vec2 gravity);
            void updateForce(Rigidbody* rigidbody, float dt) override;

            vec2 getGravity();
            void setGravity(vec2 gravity);

    };

}