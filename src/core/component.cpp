#include "pancake/core/component.hpp"
#include "pancake/core/entity.hpp"

namespace {
    int nextId = 0;
}

Component::Component() {

    this->id = nextId;
    this->entity = NULL;
    this->dead = false;

    this->keyPress = NULL;
    this->mouseButtonDown = NULL;
    //this->mouseButtonDownOnComponent = NULL;
    this->mouseDragging = NULL;
    this->callbackUpdate = false;

    nextId++;
}

Component::~Component() {
    
}

void Component::update(float dt) {

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

void Component::keyPressCallback() {
    if (this->keyPress != NULL) {this->keyPress(this);}
}

//void Component::mouseButtonDownOnComponentCallback(int button) {
//    if (this->mouseButtonDownOnComponent != NULL) {this->mouseButtonDownOnComponent(this, button);}
//}

void Component::mouseButtonDownCallback() {
    if (this->mouseButtonDown != NULL) {this->mouseButtonDown(this);}
}

void Component::mouseDraggingCallback() {
    if (this->mouseDragging != NULL) {this->mouseDragging(this);}
}

bool Component::hasKeyPressCallback() {
    return this->keyPress != NULL;
}

//bool Component::hasMouseButtonDownOnComponentCallback() {
//    return this->mouseButtonDownOnComponent != NULL;
//}

bool Component::hasMouseButtonDownCallback() {
    return this->mouseButtonDown != NULL;
}

bool Component::hasMouseDraggingCallback() {
    return this->mouseDragging != NULL;
}

bool Component::isCallbackUpdated() {
    return this->callbackUpdate;
}

void Component::setId(int id) {
    this->id = id;
}

void Component::setEntity(Entity* entity) {
    this->entity = entity;
}

void Component::setKeyPressCallback(void (*callback)(Component* component)) {
    this->keyPress = callback;
    this->callbackUpdate = true;
}

//void Component::setMouseButtonDownOnComponentCallback(void (*callback)(Component* component)) {
//    this->mouseButtonDownOnComponent = callback;
//    this->callbackUpdate = true;
//}

void Component::setMouseButtonDownCallback(void (*callback)(Component* component)) {
    this->mouseButtonDown = callback;
    this->callbackUpdate = true;
}

void Component::setMouseDraggingCallback(void (*callback)(Component* component)) {
    this->mouseDragging = callback;
    this->callbackUpdate = true;
}

void Component::clearCallbackUpdate() {
    this->callbackUpdate = false;
}

TransformableComponent::TransformableComponent() : Component() {
    this->positionOffset = glm::vec2(0.0f, 0.0f);
    this->sizeScale = glm::vec2(1.0f, 1.0f);
    this->rotationOffset = 0.0f;
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
