#include "pancake/core/component.hpp"
#include "pancake/core/entity.hpp"

namespace {
    int nextId = 0;
}

Component::Component(string type) {

    this->id = nextId;
    this->type = type;
    this->entity = NULL;
    this->dead = false;

    nextId++;
}

Component::~Component() {
    
}

void Component::update(float dt) {

}

json Component::serialise() {
    json j;
    j.emplace("id", this->id);
    j.emplace("type", this->type);
    return j;
}

void Component::onCollision(Component* with) {
    
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

bool Component::isDead() {
    return this->dead;
}

void Component::setId(int id) {
    this->id = id;
}

void Component::setEntity(Entity* entity) {
    this->entity = entity;
}

TransformableComponent::TransformableComponent(string type) : Component(type) {
    this->positionOffset = glm::vec2(0.0f, 0.0f);
    this->sizeScale = glm::vec2(1.0f, 1.0f);
    this->rotationOffset = 0.0f;
}

json TransformableComponent::serialise() {
    json j = this->Component::serialise();
    j.emplace("positionOffset", json::array());
    j["positionOffset"].push_back(this->positionOffset.x);
    j["positionOffset"].push_back(this->positionOffset.y);
    j.emplace("sizeScale", json::array());
    j["sizeScale"].push_back(this->sizeScale.x);
    j["sizeScale"].push_back(this->sizeScale.y);
    j.emplace("rotationOffset", this->rotationOffset);
    return j;
}

vec2 TransformableComponent::getPosition() {
    return this->getEntity()->getPosition() + this->positionOffset;
}

vec2 TransformableComponent::getSize() {
    return this->getEntity()->getSize() * this->sizeScale;
}

float TransformableComponent::getRotation() {
    return this->getEntity()->getRotation() + this->rotationOffset;
}

vec2 TransformableComponent::getPositionOffset() {
    return this->positionOffset;
}

vec2 TransformableComponent::getSizeScale() {
    return this->sizeScale;
}

float TransformableComponent::getRotationOffset() {
    return this->rotationOffset;
}

void TransformableComponent::setPositionOffset(vec2 offset) {
    this->positionOffset = offset;
}

void TransformableComponent::setSizeScale(vec2 scale) {
    this->sizeScale = scale;
}

void TransformableComponent::setRotationOffset(float offset) {
    this->rotationOffset = offset;
}

void TransformableComponent::addPositionOffset(vec2 offset) {
    this->positionOffset += offset;
}

void TransformableComponent::addSizeScale(vec2 scale) {
    this->sizeScale += scale;
}

void TransformableComponent::addRotationOffset(float offset) {
    this->rotationOffset += offset;
}

void KeyDown::keyDownCallback() {

}

void MouseDown::mouseDownCallback() {

}

void MouseDragging::mouseDraggingCallback() {

}