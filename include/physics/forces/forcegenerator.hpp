#pragma once

#include "physics/primitives/rigidbody.hpp"

class ForceGenerator {

    public:
        virtual void updateForce(Rigidbody* rigidbody, float dt) = 0;

};