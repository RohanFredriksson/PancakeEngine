#include "pancake/physics/gravity.hpp"

namespace Pancake {

    Gravity::Gravity(vec2 gravity) {
        this->gravity = gravity;
    }

    void Gravity::updateForce(Rigidbody* rigidbody, float dt)  {
        vec2 acceleration = this->gravity * rigidbody->getMass();
        rigidbody->addForce(acceleration);
    }

    vec2 Gravity::getGravity() {
        return this->gravity;
    }

    void Gravity::setGravity(vec2 gravity)  {
        this->gravity = gravity;
    }

}