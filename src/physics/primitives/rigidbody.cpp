#include "physics/primitives/rigidbody.hpp"
#include "window/window.hpp"

Rigidbody::Rigidbody() : Component() {

    this->collider = NULL;
    this->forceAccum = glm::vec2(0.0f, 0.0f);
    this->velocity = glm::vec2(0.0f, 0.0f);
    this->cor = 1.0f;
    this->mass = 0.0f;
    this->sensor = false;

}

Rigidbody::~Rigidbody() {
    Window::getScene()->getPhysics()->remove(this);
}

Collider* Rigidbody::getCollider() {
    return this->collider;
}

vec2 Rigidbody::getVelocity() {
    return this->velocity;
}

float Rigidbody::getCor() {
    return this->cor;
}

float Rigidbody::getMass() {
    return this->mass;
}

bool Rigidbody::isSensor() {
    return this->sensor;
}

void Rigidbody::setCollider(Collider* collider) {
    this->collider = collider;
}

void Rigidbody::setVelocity(vec2 velocity) {
    this->velocity = velocity;
}

void Rigidbody::setCor(float cor) {
    this->cor = cor;
}

void Rigidbody::setMass(float mass) {
    if (mass < 0) {mass = 0;}
    this->mass = mass;
}

void Rigidbody::setSensor(bool sensor) {
    this->sensor = sensor;
}

void Rigidbody::clearAccumulators() {
    this->forceAccum.x = 0;
    this->forceAccum.y = 0;
}

void Rigidbody::physicsUpdate(float dt) {
    
    if (this->mass == 0.0f) {return;}

    // Calculate linear velocity
    this->velocity += this->forceAccum * (dt / this->mass);

    // Update the entity's position.
    this->getEntity()->addPosition(this->velocity * dt);
    this->clearAccumulators();

}

void Rigidbody::addVelocity(vec2 velocity) {
    this->velocity += velocity;
}

void Rigidbody::addForce(vec2 force) {
    this->forceAccum += force;
}

void Rigidbody::zeroForces() {
    this->forceAccum = glm::vec2(0.0f, 0.0f);
}

bool Rigidbody::hasInfiniteMass() {
    return this->mass == 0.0f;
}