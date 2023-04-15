#include <algorithm>
#include <unordered_map>
#include "pancake/core/component.hpp"
#include "pancake/core/entity.hpp"

using std::unordered_map;

namespace {
    int nextId = 0;
}

void Component::init(int id, string type, bool load) {

    this->id = id;
    this->type = type;
    this->entity = nullptr;
    this->serialisable = true;
    this->dead = false;

    if (load) {nextId = std::max(nextId, id+1);}
    else {nextId++;}

}

Component::Component(string type) {
    this->init(nextId, type, false);
}

Component::~Component() {
    
}

void Component::start() {
    
}

void Component::update(float dt) {

}

json Component::serialise() {
    json j;
    j.emplace("id", this->id);
    j.emplace("type", this->type);
    return j;
}

bool Component::load(json j) {
    if (!j.contains("id") || !j["id"].is_number_integer()) {return false;}
    if (!j.contains("type") || !j["type"].is_string()) {return false;}
    this->init(j["id"], j["type"], true);
    return true;
}

void Component::kill() {
    this->dead = true;
}

int Component::getId() {
    return this->id;
}

string Component::getType() {
    return this->type;
}

Entity* Component::getEntity() {
    return this->entity;
}

bool Component::isSerialisable() {
    return this->serialisable;
}

bool Component::isDead() {
    return this->dead;
}

void Component::setId(int id) {
    this->id = id;
}

void Component::setEntity(Entity* entity) {
    this->entity = entity;
}

void Component::setSerialisable(bool serialisable) {
    this->serialisable = serialisable;
}

TransformableComponent::TransformableComponent(string type) : Component(type) {
    this->positionOffset = glm::vec2(0.0f, 0.0f);
    this->sizeScale = glm::vec2(1.0f, 1.0f);
    this->rotationOffset = 0.0f;
}

json TransformableComponent::serialise() {
    json j = this->Component::serialise();
    j.emplace("positionOffset", json::array());
    j["positionOffset"].push_back(this->positionOffset.x);
    j["positionOffset"].push_back(this->positionOffset.y);
    j.emplace("sizeScale", json::array());
    j["sizeScale"].push_back(this->sizeScale.x);
    j["sizeScale"].push_back(this->sizeScale.y);
    j.emplace("rotationOffset", this->rotationOffset);
    return j;
}

bool TransformableComponent::load(json j) {

    if (!this->Component::load(j)) {return false;}

    if (!j.contains("positionOffset") || !j["positionOffset"].is_array()) {return false;}
    if (j["positionOffset"].size() != 2) {return false;}
    if (!j["positionOffset"][0].is_number()) {return false;}
    if (!j["positionOffset"][1].is_number()) {return false;}

    if (!j.contains("sizeScale") || !j["sizeScale"].is_array()) {return false;}
    if (j["sizeScale"].size() != 2) {return false;}
    if (!j["sizeScale"][0].is_number()) {return false;}
    if (!j["sizeScale"][1].is_number()) {return false;}

    if (!j.contains("rotationOffset") || !j["rotationOffset"].is_number()) {return false;}

    this->setPositionOffset(vec2(j["positionOffset"][0], j["positionOffset"][1]));
    this->setSizeScale(vec2(j["sizeScale"][0], j["sizeScale"][1]));
    this->setRotationOffset(j["rotationOffset"]);

    return true;

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

namespace{
    unordered_map<string, void*(*)()> components;
}

namespace ComponentFactory {

    void add(string type, void*(*method)()) {
        components[type] = method;
    }

    Component* create(string type) {
        auto search = components.find(type);
        if (search == components.end()) {return nullptr;}
        return (Component*)(search->second());
    }

    Component* load(json j) {
        
        if (!j.contains("type") || !j["type"].is_string()) {return nullptr;}
        auto search = components.find(j["type"]);
        if (search == components.end()) {return nullptr;}

        Component* component = (Component*)(search->second());
        if (!component->load(j)) {
            delete component; // This line currently crashes the program.
            return nullptr;
        }

        return component;
    }

}