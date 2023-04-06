#include <limits>
#include "pancake/physics/primitives/box.hpp"

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