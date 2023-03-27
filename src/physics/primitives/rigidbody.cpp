#include <cmath>
#include <limits>
#include <iostream>
#include "pancake/physics/primitives/rigidbody.hpp"
#include "pancake/window/window.hpp"

Component* Rigidbody::create() {
    return new Rigidbody();
}

Rigidbody::Rigidbody() : Component("Rigidbody") {

    this->force = vec2(0.0f, 0.0f);
    this->velocity = vec2(0.0f, 0.0f);
    this->torque = 0.0f;
    this->angularVelocity = 0.0f;

    this->restitution = 1.0f;
    this->friction = 0.0f;

    this->centroid = vec2(0.0f, 0.0f);
    this->mass = 0.0f;
    this->moment = 0.0f;
    this->centroidDirty = true;
    this->massDirty = true;
    this->momentDirty = true;

    this->sensor = false;
    this->fixedOrientation = false;

}

Rigidbody::~Rigidbody() {
    Window::getScene()->getPhysics()->remove(this);
    this->clearColliders();
}

json Rigidbody::serialise() {
    json j = this->Component::serialise();
    
    j.emplace("colliders", json::array());
    for (Collider* c : this->colliders) {
        j["colliders"].push_back(c->serialise());    
    }

    j.emplace("force", json::array());
    j["force"].push_back(this->force.x);
    j["force"].push_back(this->force.y);
    
    j.emplace("velocity", json::array());
    j["velocity"].push_back(this->velocity.x);
    j["velocity"].push_back(this->velocity.y);
    
    j.emplace("torque", this->torque);
    j.emplace("angularVelocity", this->angularVelocity);
    j.emplace("restitution", this->restitution);
    j.emplace("friction", this->friction);
    j.emplace("sensor", this->sensor);
    j.emplace("fixedOrientation", this->fixedOrientation);
    
    return j;
}

vector<Collider*> Rigidbody::getColliders() {
    return this->colliders;
}

vec2 Rigidbody::getVelocity() {
    if (this->hasInfiniteMass()) {return vec2(0.0f, 0.0f);}
    return this->velocity;
}

vec2 Rigidbody::getCentroid() {

    if (!this->centroidDirty) {return this->getEntity()->getPosition() + this->centroid;}

    // If the rigidbody has infinite mass, get the average of all bodies.
    if (this->hasInfiniteMass()) {

        if (this->colliders.size() == 0) {
            std::cout << "ERROR::RIGIDBODY::GET_CENTROID::NO_COLLIDERS\n";
        }

        vec2 points = vec2(0.0f, 0.0f);
        for (Collider* c : this->colliders) {
            points += c->getPositionOffset();
        }

        vec2 average = points * (1.0f / this->colliders.size());
        this->centroid = average;
        this->centroidDirty = false;
        return this->getEntity()->getPosition() + average;
    }
    
    // Get the weighted sum of objects.
    else {
        
        float mass = 0.0f;
        vec2 weighted = vec2(0.0f, 0.0f);
        
        for (Collider* c : this->colliders) {
            mass += c->getMass();
            weighted += c->getPositionOffset() * c->getMass();
        }

        this->centroid = weighted / mass;
        this->centroidDirty = false;
        return this->getEntity()->getPosition() + this->centroid;
    }

}

float Rigidbody::getAngularVelocity() {
    if (this->hasInfiniteMass() || this->fixedOrientation) {return 0.0f;}
    return this->angularVelocity;
}

float Rigidbody::getRestitution() {
    return this->restitution;
}

float Rigidbody::getFriction() {
    return this->friction;
}

float Rigidbody::getMass() {

    if (!this->massDirty) {return this->mass;}
    
    float mass = 0.0f;
    for (Collider* c : this->colliders) {

        float m = c->getMass();
        if (m <= 0.0f) {
            this->mass = 0.0f;
            this->massDirty = false;
            return this->mass;
        }

        mass += m;
    }

    this->mass = mass;
    this->massDirty = false;
    return mass;
}

float Rigidbody::getInverseMass() {
    if (this->hasInfiniteMass()) {return 0.0f;}
    return 1.0f / this->getMass();
}

float Rigidbody::getMomentOfInertia() {

    if (!this->momentDirty) {return this->moment;}

    if (this->hasInfiniteMass()) {
        this->moment = FLT_MAX; 
        this->momentDirty = false;
        return this->moment;
    }
    
    float moment = 0.0f;
    float mass = this->getMass();
    vec2 centroid = this->getCentroid();
    for (Collider* c : this->colliders) {
        vec2 r = c->getPosition() - centroid;
        float rr = r.x * r.x + r.y * r.y;
        moment += c->getMomentOfInertia() + rr * mass;
    }

    this->moment = moment;
    this->momentDirty = false;
    return moment;
}

float Rigidbody::getInverseMomentOfInertia() {
    if (this->hasInfiniteMass()) {return 0.0f;}
    return 1.0f / this->getMomentOfInertia();
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
        this->centroidDirty = true;
        this->massDirty = true;
        this->momentDirty = true;
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
            this->centroidDirty = true;
            this->massDirty = true;
            this->momentDirty = true;
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
    this->centroidDirty = true;
    this->massDirty = true;
    this->momentDirty = true;
    return this;
}

Rigidbody* Rigidbody::setVelocity(vec2 velocity) {
    this->velocity = velocity;
    return this;
}

Rigidbody* Rigidbody::setAngularVelocity(float angularVelocity) {

    if (this->fixedOrientation) {
        this->angularVelocity = 0.0f; 
        return this;
    }

    this->angularVelocity = angularVelocity;
    return this;
}

Rigidbody* Rigidbody::setRestitution(float cor) {
    this->restitution = cor;
    return this;
}

Rigidbody* Rigidbody::setFriction(float cof) {
    this->friction = cof;
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

Rigidbody* Rigidbody::setCentroidDirty() {
    this->centroidDirty = true;
    return this;
}

Rigidbody* Rigidbody::setMassDirty() {
    this->massDirty = true;
    return this;
}

Rigidbody* Rigidbody::setMomentDirty() {
    this->momentDirty = true;
    return this;
}

void Rigidbody::clearAccumulators() {
    this->force.x = 0;
    this->force.y = 0;
    this->torque = 0;
}

void Rigidbody::physicsUpdate(float dt) {
    
    // Do not do a physics update if static.
    if (this->hasInfiniteMass()) {return;}

    // Update linear
    this->velocity += this->force * (dt / this->getMass());
    vec2 displacement = this->velocity * dt;
    this->getEntity()->addPosition(displacement);

    // Update the rotation if allowed to.
    if (!this->fixedOrientation) {

        this->angularVelocity += this->torque * (dt / this->getMass());
        float rotation = this->angularVelocity * dt;
        this->getEntity()->addRotationAround(rotation, this->getCentroid());

        // Update all collider position offsets.
        float rCos = cosf(rotation);
        float rSin = sinf(rotation);

        for (Collider* c : this->colliders) {
            vec2 offset = c->getPositionOffset();
            float x = (offset.x * rCos) - (offset.y * rSin);
            float y = (offset.x * rSin) + (offset.y * rCos);
            c->setPositionOffset(vec2(x, y), false);
        }

        // Note: There is no need to update collider rotation offsets since they are updated in the entity.
    }

    // Clear the force and torque accumulators.
    this->clearAccumulators();
    
}

void Rigidbody::addVelocity(vec2 velocity) {
    if (this->hasInfiniteMass()) {return;}
    this->velocity += velocity;
}

void Rigidbody::addAngularVelocity(float angularVelocity) {
    if (this->hasInfiniteMass() || this->fixedOrientation) {return;}
    this->angularVelocity += angularVelocity;
}

void Rigidbody::addForce(vec2 force) {
    if (this->hasInfiniteMass()) {return;}
    this->force += force;
}

void Rigidbody::zeroForces() {
    this->force = glm::vec2(0.0f, 0.0f);
}

void Rigidbody::addTorque(float torque) {
    if (this->hasInfiniteMass()) {return;}
    this->torque += torque;
}

void Rigidbody::zeroTorque() {
    this->torque = 0.0f;
}

bool Rigidbody::hasInfiniteMass() {
    float mass = this->getMass();
    if (mass <= 0.0f) {return true;}
    return false;
}