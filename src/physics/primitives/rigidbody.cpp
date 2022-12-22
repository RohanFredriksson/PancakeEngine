#include "physics/primitives/rigidbody.hpp"
#include "window/window.hpp"

Rigidbody::Rigidbody() : Component() {

    this->forceAccum = glm::vec2(0.0f, 0.0f);
    this->velocity = glm::vec2(0.0f, 0.0f);
    this->cor = 1.0f;
    this->mass = 0.0f;
    this->sensor = false;

}

Rigidbody::~Rigidbody() {
    Window::getScene()->getPhysics()->remove(this);
}

vector<Collider*> Rigidbody::getColliders() {
    return this->colliders;
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

Rigidbody* Rigidbody::addCollider(Collider* collider) {
    if (collider != NULL) {this->colliders.push_back(collider);}
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
    this->colliders.clear();
    return this;
}

Rigidbody* Rigidbody::setVelocity(vec2 velocity) {
    this->velocity = velocity;
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