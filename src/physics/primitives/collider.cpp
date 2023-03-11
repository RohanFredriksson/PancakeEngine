#include "physics/primitives/collider.hpp"

Collider::Collider() {
    this->rigidbody = NULL;
    this->positionOffset = vec2(0.0f, 0.0f);
    this->rotationOffset = 0.0f;
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
    return this->rigidbody->getEntity()->getPosition() + this->positionOffset;
}

vec2 Collider::getPositionOffset() {
    return this->positionOffset;
}

float Collider::getRotation() {
    return this->rigidbody->getEntity()->getRotation() + this->rotationOffset;
}

float Collider::getRotationOffset() {
    return this->rotationOffset;
}

Collider* Collider::setRigidbody(Rigidbody* rigidbody) {
    this->rigidbody = rigidbody;
    return this;
}

Collider* Collider::setPositionOffset(vec2 offset){
    this->positionOffset = offset;
    return this;
}

Collider* Collider::setRotationOffset(float offset) {
    this->rotationOffset = offset;
    return this;
}