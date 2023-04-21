#pragma once

#include "pancake/physics/rigidbody.hpp"

namespace Pancake {

    class ForceGenerator {

        public:
            virtual void updateForce(Rigidbody* rigidbody, float dt) = 0;

    };

}