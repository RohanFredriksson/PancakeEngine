#include "core/entity.hpp"

Component::Component() {
    
}

Component::~Component() {

}

void Component::update(float dt) {

}

void Component::kill() {
    this->dead = true;
}

int Component::getId() {
    return this->id;
}

Entity* Component::getEntity() {
    return this->entity;
}

vec2 Component::getPosition() {
    return this->entity->getPosition() + this->positionOffset;
}

vec2 Component::getSize() {
    return this->entity->getSize() + this->sizeScale;
}

float Component::getRotation() {
    return this->entity->getRotation() + this->rotationOffset;
}

vec2 Component::getPositionOffset() {
    return this->positionOffset;
}

vec2 Component::getSizeScale() {
    return this->sizeScale;
}

float Component::getRotationOffset() {
    return this->rotationOffset;
}

bool Component::isDead() {
    return this->dead;
}

void Component::setPositionOffset(vec2 offset) {
    this->positionOffset = offset;
}

void Component::setSizeScale(vec2 scale) {
    this->sizeScale = scale;
}

void Component::setRotationOffset(float offset) {
    this->rotationOffset = offset;
}

void Component::addPositionOffset(vec2 offset) {
    this->positionOffset += offset;
}

void Component::addSizeScale(vec2 scale) {
    this->sizeScale += scale;
}

void Component::addRotationOffset(float offset) {
    this->rotationOffset += offset;
}