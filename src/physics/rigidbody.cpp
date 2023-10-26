#include <cmath>
#include <limits>
#include <iostream>
#include "pancake/physics/rigidbody.hpp"
#include "pancake/core/window.hpp"

namespace Pancake {

    Rigidbody::Rigidbody() : Component("Rigidbody") {

        this->force = glm::vec2(0.0f, 0.0f);
        this->velocity = glm::vec2(0.0f, 0.0f);
        this->torque = 0.0f;
        this->angularVelocity = 0.0f;

        this->restitution = 1.0f;
        this->friction = 0.0f;

        this->lastPosition = glm::vec2(0.0f, 0.0f);
        this->lastRotation = 0.0f;

        this->centroid = glm::vec2(0.0f, 0.0f);
        this->bounds = std::make_pair(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        this->mass = 0.0f;
        this->moment = 0.0f;
        this->centroidDirty = true;
        this->boundsDirty = true;
        this->massDirty = true;
        this->momentDirty = true;

        this->sensor = false;
        this->fixedOrientation = false;

    }

    void Rigidbody::start() {
        Window::getScene()->getPhysics()->add(this);
    }

    void Rigidbody::end() {
        Window::getScene()->getPhysics()->remove(this);
        this->clearColliders();
    }

    json Rigidbody::serialise() {
        
        json j = this->Component::serialise();

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

        j.emplace("colliders", json::array());
        for (Collider* c : this->colliders) {
            j["colliders"].push_back(c->serialise());    
        }

        j.emplace("forceGenerators", json::array());
        for (const std::string& type : this->forceGenerators) {
            j["forceGenerators"].push_back(type);
        }
        
        return j;

    }

    bool Rigidbody::load(json j) {

        if (!this->Component::load(j)) {return false;}
        
        if (!j.contains("force") || !j["force"].is_array()) {return false;}
        if (j["force"].size() != 2) {return false;}
        if (!j["force"][0].is_number()) {return false;}
        if (!j["force"][1].is_number()) {return false;}

        if (!j.contains("velocity") || !j["velocity"].is_array()) {return false;}
        if (j["velocity"].size() != 2) {return false;}
        if (!j["velocity"][0].is_number()) {return false;}
        if (!j["velocity"][1].is_number()) {return false;}

        if (!j.contains("torque") || !j["torque"].is_number()) {return false;}
        if (!j.contains("angularVelocity") || !j["angularVelocity"].is_number()) {return false;}
        if (!j.contains("restitution") || !j["restitution"].is_number()) {return false;}
        if (!j.contains("friction") || !j["friction"].is_number()) {return false;}
        if (!j.contains("sensor") || !j["sensor"].is_boolean()) {return false;}
        if (!j.contains("fixedOrientation") || !j["fixedOrientation"].is_boolean()) {return false;}

        this->setForce(glm::vec2(j["force"][0], j["force"][1]));
        this->setVelocity(glm::vec2(j["velocity"][0], j["velocity"][1]));
        this->setTorque(j["torque"]);
        this->setAngularVelocity(j["angularVelocity"]);
        this->setRestitution(j["restitution"]);
        this->setFriction(j["friction"]);
        this->setSensor(j["sensor"]);
        this->setFixedOrientation(j["fixedOrientation"]);

        if (j.contains("colliders") && j["colliders"].is_array()) {
            for (auto element : j["colliders"]) {
                if (element.is_object()) {
                    if (!element.contains("type") || !element["type"].is_string()) {continue;}
                    Collider* c = FACTORY(Collider).create(element["type"]);
                    if (c == nullptr) {continue;}
                    if (!c->load(element)) {delete c; continue;}
                    else {this->addCollider(c);}
                }
            }
        }

        if (j.contains("forceGenerators") && j["forceGenerators"].is_array()) {
            for (auto element : j["forceGenerators"]) {
                if (element.is_string()) {
                    this->addForceGenerator(element);
                }
            }
        }

        return true;

    }

    std::vector<Collider*> Rigidbody::getColliders() {
        return this->colliders;
    }

    std::unordered_set<std::string> Rigidbody::getForceGenerators() {
        return this->forceGenerators;
    }

    glm::vec2 Rigidbody::getVelocity() {
        if (this->hasInfiniteMass()) {return glm::vec2(0.0f, 0.0f);}
        return this->velocity;
    }

    glm::vec2 Rigidbody::getCentroid() {

        if (!this->centroidDirty) {return this->getEntity()->getPosition() + this->centroid;}

        // If the rigidbody has infinite mass, get the average of all bodies.
        if (this->hasInfiniteMass()) {

            if (this->colliders.size() == 0) {
                std::cout << "ERROR::RIGIDBODY::GET_CENTROID::NO_COLLIDERS\n";
            }

            glm::vec2 points = glm::vec2(0.0f, 0.0f);
            for (Collider* c : this->colliders) {
                points += c->getPositionOffset();
            }

            glm::vec2 average = points * (1.0f / this->colliders.size());
            this->centroid = average;
            this->centroidDirty = false;
            return this->getEntity()->getPosition() + average;
        }
        
        // Get the weighted sum of objects.
        else {
            
            float mass = 0.0f;
            glm::vec2 weighted = glm::vec2(0.0f, 0.0f);
            
            for (Collider* c : this->colliders) {
                mass += c->getMass();
                weighted += c->getPositionOffset() * c->getMass();
            }

            this->centroid = weighted / mass;
            this->centroidDirty = false;
            return this->getEntity()->getPosition() + this->centroid;
        }

    }

    std::pair<glm::vec2, glm::vec2> Rigidbody::getBounds() {

        if (!this->boundsDirty) {return this->bounds;}

        glm::vec2 first = glm::vec2(0.0f, 0.0f);
        glm::vec2 second = glm::vec2(0.0f, 0.0f);

        for (Collider* c : this->colliders) {

            std::pair<glm::vec2, glm::vec2> bounds = c->getLocalBounds();
            glm::vec2 offset = c->getPositionOffset();

            glm::vec2 min = offset + bounds.first;
            glm::vec2 max = offset + bounds.second;

            first.x = std::min(first.x, min.x);
            first.y = std::min(first.y, min.y);
            second.x = std::max(second.x, max.x);
            second.y = std::max(second.y, max.y);

        }

        glm::vec2 position = this->getEntity()->getPosition();
        this->bounds = std::make_pair(position + first, position + second);
        this->boundsDirty = false;
        return this->bounds;

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
        glm::vec2 centroid = this->getCentroid();
        for (Collider* c : this->colliders) {
            glm::vec2 r = c->getPosition() - centroid;
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

    bool Rigidbody::hasForceGenerator(std::string type) {
        auto it = this->forceGenerators.find(type);
        return it != this->forceGenerators.end();
    }

    bool Rigidbody::hasFixedOrientation() {
        return this->fixedOrientation;
    }

    Rigidbody* Rigidbody::addCollider(Collider* collider) {
        if (collider != nullptr) {
            this->colliders.push_back(collider);
            collider->setRigidbody(this);
            this->centroidDirty = true;
            this->boundsDirty = true;
            this->massDirty = true;
            this->momentDirty = true;
        }
        return this;
    }

    Rigidbody* Rigidbody::addColliders(std::vector<Collider*> colliders) {
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
                this->boundsDirty = true;
                this->massDirty = true;
                this->momentDirty = true;
                break;
            }
        }
        return this;
    }

    Rigidbody* Rigidbody::removeColliders(std::vector<Collider*> colliders) {
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

    Rigidbody* Rigidbody::setColliders(std::vector<Collider*> colliders) {
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
        this->boundsDirty = true;
        this->massDirty = true;
        this->momentDirty = true;
        return this;
    }

    Rigidbody* Rigidbody::addForceGenerator(std::string type) {
        this->forceGenerators.insert(type);
        Scene* scene = Window::getScene();
        if (scene != nullptr) {scene->getPhysics()->addForceRegistration(type, this);}
        return this;
    }

    Rigidbody* Rigidbody::addForceGenerators(std::vector<std::string> types) {
        for (std::string type : types) {this->addForceGenerator(type);}
        return this;
    }
    
    Rigidbody* Rigidbody::removeForceGenerator(std::string type) {
        this->forceGenerators.erase(type);
        Scene* scene = Window::getScene();
        if (scene != nullptr) {scene->getPhysics()->removeForceRegistration(type, this);}
        return this;
    }
    
    Rigidbody* Rigidbody::removeForceGenerators(std::vector<std::string> types) {
        for (std::string type : types) {this->removeForceGenerator(type);}
        return this;
    }
    
    Rigidbody* Rigidbody::setForceGenerator(std::string type) {
        this->clearForceGenerators();
        this->addForceGenerator(type);
        return this;
    }
    
    Rigidbody* Rigidbody::setForceGenerators(std::vector<std::string> types) {
        this->clearForceGenerators();
        this->addForceGenerators(types);
        return this;
    }
    
    Rigidbody* Rigidbody::clearForceGenerators() {
        this->forceGenerators.clear();
        return this;
    }

    Rigidbody* Rigidbody::setForce(glm::vec2 force) {
        if (this->hasInfiniteMass()) {return this;}
        this->force = force;
        return this;
    }

    Rigidbody* Rigidbody::setForce(float x, float y) {
        return this->setForce(glm::vec2(x, y));
    }

    Rigidbody* Rigidbody::setVelocity(glm::vec2 velocity) {
        if (this->hasInfiniteMass()) {return this;}
        this->velocity = velocity;
        return this;
    }

    Rigidbody* Rigidbody::setVelocity(float x, float y) {
        return this->setVelocity(glm::vec2(x, y));
    }

    Rigidbody* Rigidbody::setTorque(float torque) {
        this->torque = torque;
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

    Rigidbody* Rigidbody::setBoundsDirty() {
        this->boundsDirty = true;
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

        // Required variables
        glm::vec2 displacement = glm::vec2(0.0f, 0.0f);
        float rotation = 0.0f;

        // Update linear
        this->velocity += this->force * (dt / this->getMass());
        displacement = this->velocity * dt;
        this->getEntity()->addPosition(displacement);

        // Update the rotation if allowed to.
        if (!this->fixedOrientation) {

            this->angularVelocity += this->torque * (dt / this->getMass());
            rotation = this->angularVelocity * dt;
            this->getEntity()->addRotationAround(rotation, this->getCentroid());

            // Update all collider position offsets.
            float rCos = cosf(rotation);
            float rSin = sinf(rotation);

            for (Collider* c : this->colliders) {
                glm::vec2 offset = c->getPositionOffset();
                float x = (offset.x * rCos) - (offset.y * rSin);
                float y = (offset.x * rSin) + (offset.y * rCos);
                c->setPositionOffset(glm::vec2(x, y), false);
            }

            // Note: There is no need to update collider rotation offsets since they are updated in the entity.
        }

        // Clear the force and torque accumulators.
        this->clearAccumulators();

        // Check if there has been a changed in the bounding box.
        this->boundsDirty = (this->lastPosition != this->getEntity()->getPosition()) || 
                            (this->lastRotation != this->getEntity()->getRotation()) ||
                            (displacement != glm::vec2(0.0f, 0.0f)) || 
                            (rotation != 0.0f) || 
                            (this->boundsDirty);

        // If the bounding box has changed we need to update the spatial hash grid.
        if (this->boundsDirty) {
            std::pair<glm::vec2, glm::vec2> bounds = this->getBounds();
            Window::getScene()->getPhysics()->getGrid()->update(this, bounds.first, bounds.second);
        }
    
        // Update last position and rotation
        this->lastPosition = this->getEntity()->getPosition();
        this->lastRotation = this->getEntity()->getRotation();

    }

    void Rigidbody::addVelocity(glm::vec2 velocity) {
        if (this->hasInfiniteMass()) {return;}
        this->velocity += velocity;
    }

    void Rigidbody::addVelocity(float x, float y) {
        this->addVelocity(glm::vec2(x, y));
    }

    void Rigidbody::addAngularVelocity(float angularVelocity) {
        if (this->hasInfiniteMass() || this->fixedOrientation) {return;}
        this->angularVelocity += angularVelocity;
    }

    void Rigidbody::addForce(glm::vec2 force) {
        if (this->hasInfiniteMass()) {return;}
        this->force += force;
    }

    void Rigidbody::addForce(float x, float y) {
        this->addForce(glm::vec2(x, y));
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

}