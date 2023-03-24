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
    this->keyBeginPress = NULL;
    this->mouseDown = NULL;
    this->mouseBeginDown = NULL;
    this->mouseDownOnComponent = NULL;
    this->mouseBeginDownOnComponent = NULL;
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

void Component::keyPressCallback(int key) {
    if (this->keyPress != NULL) {this->keyPress(this, key);}
}

void Component::keyBeginPressCallback(int key) {
    if (this->keyBeginPress != NULL) {this->keyBeginPress(this, key);}
}

void Component::mouseDownOnComponentCallback(int button) {
    if (this->mouseDownOnComponent != NULL) {this->mouseDownOnComponent(this, button);}
}

void Component::mouseBeginDownOnComponentCallback(int button) {
    if (this->mouseBeginDownOnComponent != NULL) {this->mouseBeginDownOnComponent(this, button);}
}

void Component::mouseDownCallback(int button) {
    if (this->mouseDown != NULL) {this->mouseDown(this, button);}
}

void Component::mouseBeginDownCallback(int button) {
    if (this->mouseBeginDown != NULL) {this->mouseBeginDown(this, button);}
}

void Component::mouseDraggingCallback() {
    if (this->mouseDragging != NULL) {this->mouseDragging(this);}
}

bool Component::hasKeyPressCallback() {
    return this->keyPress != NULL;
}

bool Component::hasKeyBeginPressCallback() {
    return this->keyBeginPress != NULL;
}

bool Component::hasMouseDownOnComponentCallback() {
    return this->mouseDownOnComponent != NULL;
}

bool Component::hasMouseBeginDownOnComponentCallback() {
    return this->mouseBeginDownOnComponent != NULL;
}

bool Component::hasMouseDownCallback() {
    return this->mouseDown != NULL;
}

bool Component::hasMouseBeginDownCallback() {
    return this->mouseBeginDown != NULL;
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

void Component::setKeyPressCallback(void (*callback)(Component* component, int key)) {
    this->keyPress = callback;
    this->callbackUpdate = true;
}

void Component::setKeyBeginPressCallback(void (*callback)(Component* component, int key)) {
    this->keyBeginPress = callback;
    this->callbackUpdate = true;
}

void Component::setMouseDownOnComponentCallback(void (*callback)(Component* component, int button)) {
    this->mouseDownOnComponent = callback;
    this->callbackUpdate = true;
}

void Component::setMouseBeginDownOnComponentCallback(void (*callback)(Component* component, int button)) {
    this->mouseBeginDownOnComponent = callback;
    this->callbackUpdate = true;
}

void Component::setMouseDownCallback(void (*callback)(Component* component, int button)) {
    this->mouseDown = callback;
    this->callbackUpdate = true;
}

void Component::setMouseBeginDownCallback(void (*callback)(Component* component, int button)) {
    this->mouseBeginDown = callback;
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
