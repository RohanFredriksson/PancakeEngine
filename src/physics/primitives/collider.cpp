#include "physics/primitives/collider.hpp"

Collider::Collider() : Component() {
    this->rigidbody = NULL;
}

Collider::~Collider() {
    
}

Rigidbody* Collider::getRigidbody() {
    return this->rigidbody;
}

Collider* Collider::setRigidbody(Rigidbody* rigidbody) {
    this->rigidbody = rigidbody;
    return this;
}