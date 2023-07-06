#include <iostream>
#include <unordered_map>
#include "pancake/physics/collider.hpp"

using std::unordered_map;

namespace Pancake {

    void Collider::init(string type, float mass, vec2 positionOffset, float rotationOffset) {
        this->rigidbody = nullptr;
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

        if (this->rigidbody != nullptr) {
            this->rigidbody->setCentroidDirty();
            this->rigidbody->setMomentDirty();
            this->rigidbody->setMassDirty();
        }

        if (mass < 0.0f) {std::cout << "WARNING::COLLIDER::SET_MASS::NEGATIVE_MASS\n";}
        this->mass = mass;
        return this;
    }

    Collider* Collider::setPositionOffset(vec2 offset){

        if (this->rigidbody != nullptr) {
            this->rigidbody->setCentroidDirty();
            this->rigidbody->setMomentDirty();
        }

        this->positionOffset = offset;
        return this;
    }

    Collider* Collider::setRotationOffset(float offset) {

        if (this->rigidbody != nullptr) {
            this->rigidbody->setCentroidDirty();
            this->rigidbody->setMomentDirty();
        }

        this->rotationOffset = offset;
        return this;
    }

    Collider* Collider::setPositionOffset(vec2 offset, bool update) {

        if (update && this->rigidbody != nullptr) {
            this->rigidbody->setCentroidDirty();
            this->rigidbody->setMomentDirty();
        }

        this->positionOffset = offset;
        return this;
    }

    Collider* Collider::setRotationOffset(float offset, bool update) {

        if (update && this->rigidbody != nullptr) {
            this->rigidbody->setCentroidDirty();
            this->rigidbody->setMomentDirty();
        }

        this->rotationOffset = offset;
        return this;
    }

    Box::Box() : Collider("Box")  {
        this->size = vec2(1.0f, 1.0f);
        this->halfSize = this->size * 0.5f;
    }

    json Box::serialise() {

        json j = this->Collider::serialise();

        j.emplace("size", json::array());
        j["size"].push_back(this->size.x);
        j["size"].push_back(this->size.y);

        return j;
        
    }

    bool Box::load(json j) {
        
        if (!this->Collider::load(j)) {return false;}

        if (!j.contains("size") || !j["size"].is_array()) {return false;}
        if (j["size"].size() != 2) {return false;}
        if (!j["size"][0].is_number()) {return false;}
        if (!j["size"][1].is_number()) {return false;}

        this->setSize(vec2(j["size"][0], j["size"][1]));
        
        return true;

    }

    float Box::getMomentOfInertia() {
        if (this->mass <= 0.0f) {return FLT_MAX;}
        return this->mass * (this->size.x * this->size.x + this->size.y * this->size.y) / 12.0f;
    }

    vec2 Box::getSize() {
        return this->size;
    }

    vec2 Box::getHalfSize() {
        return this->halfSize;
    }

    vec2 Box::getMin() {
        return this->getPosition() - this->halfSize;
    }

    vec2 Box::getMax() {
        return this->getPosition() + this->halfSize;
    }

    Box* Box::setSize(vec2 size) {
        this->size = size;
        this->halfSize = size * 0.5f;
        return this;
    }

    Circle::Circle() : Collider("Circle") {
        this->radius = 0.5f;
    }

    json Circle::serialise() {
        json j = this->Collider::serialise();
        j.emplace("radius", this->radius);
        return j;
    }

    bool Circle::load(json j) {
        if (!this->Collider::load(j)) {return false;}
        if (!j.contains("radius") || !j["radius"].is_number()) {return false;}
        this->setRadius(j["radius"]);
        return true;
    }

    float Circle::getMomentOfInertia() {
        if (this->mass <= 0.0f) {return FLT_MAX;}
        return 0.5f * this->mass * this->radius * this->radius;
    }

    float Circle::getRadius() {
        return this->radius;
    }

    Circle* Circle::setRadius(float radius) {
        this->radius = radius;
        return this;
    }

}