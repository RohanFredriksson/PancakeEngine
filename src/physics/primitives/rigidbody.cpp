#include <limits>
#include "physics/primitives/rigidbody.hpp"
#include "window/window.hpp"

Rigidbody::Rigidbody() : Component() {

    this->forceAccum = glm::vec2(0.0f, 0.0f);
    this->velocity = glm::vec2(0.0f, 0.0f);
    this->torqueAccum = 0.0f;
    this->angularVelocity = 0.0f;
    this->cor = 1.0f;
    this->mass = 0.0f;
    this->sensor = false;
    this->fixedOrientation = false;

}

Rigidbody::~Rigidbody() {
    Window::getScene()->getPhysics()->remove(this);
    this->clearColliders();
}

vector<Collider*> Rigidbody::getColliders() {
    return this->colliders;
}

vec2 Rigidbody::getVelocity() {
    if (this->hasInfiniteMass()) {return vec2(0.0f, 0.0f);}
    return this->velocity;
}

float Rigidbody::getAngularVelocity() {
    if (this->hasInfiniteMass()) {return 0.0f;}
    return this->angularVelocity;
}

float Rigidbody::getCor() {
    return this->cor;
}

float Rigidbody::getMass() {
    return this->mass;
}

float Rigidbody::getInverseMass() {
    if (this->hasInfiniteMass()) {return 0.0f;}
    return 1.0f / this->mass;
}

float Rigidbody::getMomentOfInertia() {
    if (this->hasInfiniteMass()) {return FLT_MAX;}
    vec2 size = this->getEntity()->getSize();
    return this->mass * (size.x * size.x + size.y * size.y) / 12.0f;
}

float Rigidbody::getInverseMomentOfInertia() {
    if (this->hasInfiniteMass()) {return 0.0f;}
    vec2 size = this->getEntity()->getSize();
    return 12.0f / (this->mass * (size.x * size.x + size.y * size.y));
}

bool Rigidbody::isSensor() {
    return this->sensor;
}

bool Rigidbody::hasFixedOrientation() {
    return this->fixedOrientation;
}

Rigidbody* Rigidbody::addCollider(Collider* collider) {
    if (collider != NULL) {
        this->colliders.push_back(collider);
        collider->setRigidbody(this);
    }
    return this;
}

Rigidbody* Rigidbody::addColliders(vector<Collider*> colliders) {
    int n = colliders.size();
    for (int i = 0; i < n; i++) {
        this->addCollider(colliders[i]);
    }
    return this;
}

Rigidbody* Rigidbody::removeCollider(Collider* collider) {
    int n = colliders.size();
    for (int i = 0; i < n; i++) {
        if (this->colliders[i] == collider) {
            this->colliders.erase(this->colliders.begin() + i);
            delete collider;
            break;
        }
    }
    return this;
}

Rigidbody* Rigidbody::removeColliders(vector<Collider*> colliders) {
    int n = colliders.size();
    for (int i = 0; i < n; i++) {
        this->removeCollider(colliders[i]);
    }
    return this;
}

Rigidbody* Rigidbody::setCollider(Collider* collider) {
    this->clearColliders();
    this->addCollider(collider);
    return this;
}

Rigidbody* Rigidbody::setColliders(vector<Collider*> colliders) {
    this->clearColliders();
    this->addColliders(colliders);
    return this;
}

Rigidbody* Rigidbody::clearColliders() {
    int n = this->colliders.size();
    for (int i = 0; i < n; i++) {
        delete this->colliders[i];
    }
    this->colliders.clear();
    return this;
}

Rigidbody* Rigidbody::setVelocity(vec2 velocity) {
    this->velocity = velocity;
    return this;
}

Rigidbody* Rigidbody::setAngularVelocity(float angularVelocity) {
    this->angularVelocity = angularVelocity;
    return this;
}

Rigidbody* Rigidbody::setCor(float cor) {
    this->cor = cor;
    return this;
}

Rigidbody* Rigidbody::setMass(float mass) {
    if (mass < 0) {mass = 0;}
    this->mass = mass;
    return this;
}

Rigidbody* Rigidbody::setSensor(bool sensor) {
    this->sensor = sensor;
    return this;
}

Rigidbody* Rigidbody::setFixedOrientation(bool orientation) {
    this->fixedOrientation = orientation;
    return this;
}

void Rigidbody::clearAccumulators() {
    this->forceAccum.x = 0;
    this->forceAccum.y = 0;
    this->torqueAccum = 0;
}

void Rigidbody::physicsUpdate(float dt) {
    
    // Do not do a physics update if static.
    if (this->hasInfiniteMass()) {return;}

    // Calculate velocity
    this->velocity += this->forceAccum * (dt / this->mass);
    this->angularVelocity += this->torqueAccum * (dt / this->mass);

    // Calculate displacement
    vec2 displacement = this->velocity * dt;
    float rotation = this->angularVelocity * dt;

    // Clear the force and torque accumulators.
    this->clearAccumulators();

    // Update the entity's position.
    this->getEntity()->addPosition(displacement);

    // Update the rotation if required and allowed to.
    if (!this->fixedOrientation && rotation != 0.0f) {

        // Update the entity's rotation.
        this->getEntity()->addRotation(rotation);

        // Update all collider position offsets.
        float radians = rotation * M_PI / 180.0f;
        float rCos = (float) cos(radians);
        float rSin = (float) sin(radians);

        for (Collider* c : this->colliders) {
            vec2 offset = c->getPositionOffset();
            float x = (offset.x * rCos) - (offset.y * rSin);
            float y = (offset.x * rSin) + (offset.y * rCos);
            c->setPositionOffset(vec2(x, y));
        }

        // Note: There is no need to update collider rotation offsets since they are updated in the entity.
    }
    
}

void Rigidbody::addVelocity(vec2 velocity) {
    if (this->hasInfiniteMass()) {return;}
    this->velocity += velocity;
}

void Rigidbody::addAngularVelocity(float angularVelocity) {
    if (this->hasInfiniteMass()) {return;}
    this->angularVelocity += angularVelocity;
}

void Rigidbody::addForce(vec2 force) {
    if (this->hasInfiniteMass()) {return;}
    this->forceAccum += force;
}

void Rigidbody::zeroForces() {
    this->forceAccum = glm::vec2(0.0f, 0.0f);
}

void Rigidbody::addTorque(float torque) {
    if (this->hasInfiniteMass()) {return;}
    this->torqueAccum += torque;
}

void Rigidbody::zeroTorque() {
    this->torqueAccum = 0.0f;
}

bool Rigidbody::hasInfiniteMass() {
    return this->mass == 0.0f;
}