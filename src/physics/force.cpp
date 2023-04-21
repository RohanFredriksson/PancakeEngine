#include "pancake/physics/force.hpp"

namespace Pancake {

    ForceRegistration::ForceRegistration(ForceGenerator* generator, Rigidbody* rigidbody) {
        this->generator = generator;
        this->rigidbody = rigidbody;
    }

    void ForceRegistry::add(ForceGenerator* generator, Rigidbody* rigidbody) {
        ForceRegistration registration = ForceRegistration(generator, rigidbody);
        this->registry.push_back(registration);
    }

    void ForceRegistry::remove(ForceGenerator* generator, Rigidbody* rigidbody) {
        int n = this->registry.size();
        for (int i = 0; i < n; i++) {
            ForceRegistration registration = this->registry[i];
            if (registration.generator == generator && registration.rigidbody == rigidbody) {
                this->registry.erase(this->registry.begin() + i);
                return;
            }
        }
    }

    void ForceRegistry::clear() {
        this->registry.clear();
    }

    void ForceRegistry::updateForces(float dt) {
        int n = this->registry.size();
        for (int i = 0; i < n; i++) {
            ForceRegistration registration = this->registry[i];
            registration.generator->updateForce(registration.rigidbody, dt);
        }
    }

    void ForceRegistry::zeroForces() {
        int n = this->registry.size();
        for (int i = 0; i < n; i++) {
            ForceRegistration registration = this->registry[i];
            registration.rigidbody->zeroForces();
        }
    }

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