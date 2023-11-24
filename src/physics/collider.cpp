#include <cmath>
#include <iostream>
#include <stdexcept>
#include "pancake/physics/collider.hpp"

namespace Pancake {

    namespace {

        void rotateVector(vec2& vec, vec2 origin, float rCos, float rSin) {
            float x = vec.x - origin.x;
            float y = vec.y - origin.y;
            vec.x = origin.x + ((x * rCos) - (y * rSin));
            vec.y = origin.y + ((x * rSin) + (y * rCos));
        }

    }

    void Collider::init(std::string type, float mass, glm::vec2 positionOffset, float rotationOffset) {
        this->rigidbody = nullptr;
        this->type = type;
        this->mass = mass;
        this->positionOffset = positionOffset;
        this->rotationOffset = rotationOffset;
    }

    Collider::Collider(std::string type) {
        this->init(type, 0.0f, glm::vec2(0.0f, 0.0f), 0.0f);
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

        this->init(j["type"], j["mass"], glm::vec2(j["positionOffset"][0], j["positionOffset"][1]), j["rotationOffset"]);

        return true;

    }

    string Collider::getType() {
        return this->type;
    }

    Rigidbody* Collider::getRigidbody() {
        return this->rigidbody;
    }

    float Collider::getMass() {
        return this->mass;
    }

    float Collider::getMomentOfInertia() {
        std::cout << "ERROR::COLLIDER::GET_MOMENT_OF_INERTIA::UNDEFINED\n";
        throw std::runtime_error("ERROR::COLLIDER::GET_MOMENT_OF_INERTIA::UNDEFINED");
        return FLT_MAX;
    }

    std::pair<glm::vec2, glm::vec2> Collider::getLocalBounds() {
        std::cout << "ERROR::COLLIDER::GET_MOMENT_OF_GETBOUNDS::UNDEFINED\n";
        throw std::runtime_error("ERROR::COLLIDER::GET_MOMENT_OF_GETBOUNDS::UNDEFINED");
        return std::make_pair(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
    }

    glm::vec2 Collider::getPosition() {
        return this->rigidbody->getEntity()->getPosition() + this->positionOffset;
    }

    glm::vec2 Collider::getPositionOffset() {
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

    Collider* Collider::setPositionOffset(float x, float y) {
        return this->setPositionOffset(glm::vec2(x, y));
    }

    Collider* Collider::setPositionOffset(glm::vec2 offset){

        if (this->rigidbody != nullptr) {
            this->rigidbody->setCentroidDirty();
            this->rigidbody->setBoundsDirty();
            this->rigidbody->setMomentDirty();
        }

        this->positionOffset = offset;
        return this;
    }

    Collider* Collider::setRotationOffset(float offset) {

        if (this->rigidbody != nullptr) {
            this->rigidbody->setCentroidDirty();
            this->rigidbody->setBoundsDirty();
            this->rigidbody->setMomentDirty();
        }

        this->rotationOffset = offset;
        return this;
    }

    Collider* Collider::setPositionOffset(glm::vec2 offset, bool update) {

        if (update && this->rigidbody != nullptr) {
            this->rigidbody->setCentroidDirty();
            this->rigidbody->setBoundsDirty();
            this->rigidbody->setMomentDirty();
        }

        this->positionOffset = offset;
        return this;
    }

    Collider* Collider::setRotationOffset(float offset, bool update) {

        if (update && this->rigidbody != nullptr) {
            this->rigidbody->setCentroidDirty();
            this->rigidbody->setBoundsDirty();
            this->rigidbody->setMomentDirty();
        }

        this->rotationOffset = offset;
        return this;
    }

    BoxCollider::BoxCollider() : Collider("BoxCollider")  {
        this->size = glm::vec2(1.0f, 1.0f);
    }

    json BoxCollider::serialise() {

        json j = this->Collider::serialise();

        j.emplace("size", json::array());
        j["size"].push_back(this->size.x);
        j["size"].push_back(this->size.y);

        return j;
        
    }

    bool BoxCollider::load(json j) {
        
        if (!this->Collider::load(j)) {return false;}

        if (!j.contains("size") || !j["size"].is_array()) {return false;}
        if (j["size"].size() != 2) {return false;}
        if (!j["size"][0].is_number()) {return false;}
        if (!j["size"][1].is_number()) {return false;}

        this->setSize(glm::vec2(j["size"][0], j["size"][1]));
        
        return true;

    }

    float BoxCollider::getMomentOfInertia() {
        if (this->getMass() <= 0.0f) {return FLT_MAX;}
        return this->getMass() * (this->size.x * this->size.x + this->size.y * this->size.y) / 12.0f;
    }

    std::pair<glm::vec2, glm::vec2> BoxCollider::getLocalBounds() {
        
        // Rotate two corners to determine the aabb.
        glm::vec2 t = this->size * 0.5f; // Top Right Corner
        glm::vec2 b = t; b.y *= -1.0f; // Bottom Right Corner

        // Required variables to compute rotation.
        float r = this->getRotation();
        float c = cosf(r);
        float s = sinf(r);

        // Rotate the vectors.
        rotateVector(t, glm::vec2(0.0f, 0.0f), c, s);
        rotateVector(b, glm::vec2(0.0f, 0.0f), c, s);

        // Find the absolute magnitude.
        t.x = std::abs(t.x);
        t.y = std::abs(t.y);
        b.x = std::abs(b.x);
        b.y = std::abs(b.y);

        // Take the maximum of t and b.
        t.x = std::max(t.x, b.x);
        t.y = std::max(t.y, b.y);

        // Compute the aabb in the rigidbody's local space.
        return std::make_pair(-t, t);

    }

    glm::vec2 BoxCollider::getSize() {
        return this->size;
    }

    BoxCollider* BoxCollider::setSize(glm::vec2 size) {

        if (this->getRigidbody() != nullptr) {
            this->getRigidbody()->setBoundsDirty();
            this->getRigidbody()->setMomentDirty();
        }

        this->size = size;
        return this;

    }

    BoxCollider* BoxCollider::setSize(float w, float h) {
        return this->setSize(glm::vec2(w, h));
    }

    CircleCollider::CircleCollider() : Collider("CircleCollider") {
        this->radius = 0.5f;
    }

    json CircleCollider::serialise() {
        json j = this->Collider::serialise();
        j.emplace("radius", this->radius);
        return j;
    }

    bool CircleCollider::load(json j) {
        if (!this->Collider::load(j)) {return false;}
        if (!j.contains("radius") || !j["radius"].is_number()) {return false;}
        this->setRadius(j["radius"]);
        return true;
    }

    float CircleCollider::getMomentOfInertia() {
        if (this->getMass() <= 0.0f) {return FLT_MAX;}
        return 0.5f * this->getMass() * this->radius * this->radius;
    }

    std::pair<glm::vec2, glm::vec2> CircleCollider::getLocalBounds() {
        return std::make_pair(glm::vec2(-this->radius, -this->radius), glm::vec2(this->radius, this->radius));
    }

    float CircleCollider::getRadius() {
        return this->radius;
    }

    CircleCollider* CircleCollider::setRadius(float radius) {

        if (this->getRigidbody() != nullptr) {
            this->getRigidbody()->setBoundsDirty();
            this->getRigidbody()->setMomentDirty();
        }

        this->radius = radius;
        return this;

    }

}