#include "physics/primitives/collider.hpp"

Collider::Collider() {
    this->rigidbody = NULL;
    this->positionOffset = vec2(0.0f, 0.0f);
}

Collider::~Collider() {
    
}

Rigidbody* Collider::getRigidbody() {
    return this->rigidbody;
}

Entity* Collider::getEntity() {
    return this->rigidbody->getEntity();
}

vec2 Collider::getPosition() {
    return this->rigidbody->getPosition() + this->positionOffset;
}

vec2 Collider::getPositionOffset() {
    return this->positionOffset;
}

Collider* Collider::setRigidbody(Rigidbody* rigidbody) {
    this->rigidbody = rigidbody;
    return this;
}

Collider* Collider::setPositionOffset(vec2 offset){
    this->positionOffset = offset;
    return this;
}