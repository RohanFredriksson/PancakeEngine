#pragma once

#include "pancake/physics/rigidbody.hpp"

class ForceGenerator {

    public:
        virtual void updateForce(Rigidbody* rigidbody, float dt) = 0;

};