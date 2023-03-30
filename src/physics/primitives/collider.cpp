#include <iostream>
#include <unordered_map>
#include "pancake/physics/primitives/collider.hpp"

using std::unordered_map;

void Collider::init(string type, float mass, vec2 positionOffset, float rotationOffset) {
    this->rigidbody = NULL;
    this->type = type;
    this->mass = mass;
    this->positionOffset = positionOffset;
    this->rotationOffset = rotationOffset;
}

Collider::Collider(string type) {
    this->init(type, 0.0f, vec2(0.0f, 0.0f), 0.0f);
}

Collider::~Collider() {

}

json Collider::serialise() {

    json j;

    j.emplace("type", this->type);
    j.emplace("mass", this->mass);

    j.emplace("positionOffset", json::array());
    j["positionOffset"].push_back(this->positionOffset.x);
    j["positionOffset"].push_back(this->positionOffset.y);

    j.emplace("rotationOffset", this->rotationOffset);

    return j;

}

bool Collider::load(json j) {
    
    if (!j.contains("type") || !j["type"].is_string()) {return false;}
    if (!j.contains("mass") || !j["mass"].is_number()) {return false;}
    
    if (!j.contains("positionOffset") || !j["positionOffset"].is_array()) {return false;}
    if (j["positionOffset"].size() != 2) {return false;}
    if (!j["positionOffset"][0].is_number()) {return false;}
    if (!j["positionOffset"][1].is_number()) {return false;}

    if (!j.contains("rotationOffset") || !j["rotationOffset"].is_number()) {return false;}

    this->init(j["type"], j["mass"], vec2(j["positionOffset"][0], j["positionOffset"][1]), j["rotationOffset"]);

    return true;

}

Rigidbody* Collider::getRigidbody() {
    return this->rigidbody;
}

float Collider::getMass() {
    return this->mass;
}

float Collider::getMomentOfInertia() {
    return FLT_MAX;
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

bool Collider::hasInfiniteMass() {
    return this->mass <= 0.0f;
}

Collider* Collider::setRigidbody(Rigidbody* rigidbody) {
    this->rigidbody = rigidbody;
    return this;
}

Collider* Collider::setMass(float mass) {

    if (this->rigidbody != NULL) {
        this->rigidbody->setCentroidDirty();
        this->rigidbody->setMomentDirty();
        this->rigidbody->setMassDirty();
    }

    if (mass < 0.0f) {std::cout << "WARNING::COLLIDER::SET_MASS::NEGATIVE_MASS\n";}
    this->mass = mass;
    return this;
}

Collider* Collider::setPositionOffset(vec2 offset){

    if (this->rigidbody != NULL) {
        this->rigidbody->setCentroidDirty();
        this->rigidbody->setMomentDirty();
    }

    this->positionOffset = offset;
    return this;
}

Collider* Collider::setRotationOffset(float offset) {

    if (this->rigidbody != NULL) {
        this->rigidbody->setCentroidDirty();
        this->rigidbody->setMomentDirty();
    }

    this->rotationOffset = offset;
    return this;
}

Collider* Collider::setPositionOffset(vec2 offset, bool update) {

    if (update && this->rigidbody != NULL) {
        this->rigidbody->setCentroidDirty();
        this->rigidbody->setMomentDirty();
    }

    this->positionOffset = offset;
    return this;
}

Collider* Collider::setRotationOffset(float offset, bool update) {

    if (update && this->rigidbody != NULL) {
        this->rigidbody->setCentroidDirty();
        this->rigidbody->setMomentDirty();
    }

    this->rotationOffset = offset;
    return this;
}

namespace {
    unordered_map<string, Collider*(*)()> colliders;
}

namespace ColliderFactory {

    void add(string type, Collider*(*method)()) {
        colliders[type] = method;
    }

    Collider* create(string type) {
        auto search = colliders.find(type);
        if (search == colliders.end()) {return NULL;}
        return search->second();
    }

    Collider* load(json j) {
        
        if (!j.contains("type") || !j["type"].is_string()) {return NULL;}
        auto search = colliders.find(j["type"]);
        if (search == colliders.end()) {return NULL;}
        
        Collider* collider = search->second();
        if (!collider->load(j)) {
            delete collider; 
            return NULL;
        }

        return collider;
    }

}