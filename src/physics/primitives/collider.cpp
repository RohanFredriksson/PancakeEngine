#include "physics/primitives/collider.hpp"

Collider::Collider() : Component() {

}

Collider::~Collider() {

}

Rigidbody* Collider::getRigidbody() {
    return this->rigidbody;
}

void Collider::setRigidbody(Rigidbody* rigidbody) {
    this->rigidbody = rigidbody;
}