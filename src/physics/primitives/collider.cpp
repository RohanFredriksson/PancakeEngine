#include "physics/primitives/collider.hpp"

Collider::Collider() : Component() {
    this->rigidbody = NULL;
}

Rigidbody* Collider::getRigidbody() {
    return this->rigidbody;
}

void Collider::setRigidbody(Rigidbody* rigidbody) {
    this->rigidbody = rigidbody;
}