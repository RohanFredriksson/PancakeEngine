#include "physics/forces/forceregistry.hpp"

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