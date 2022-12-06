#include "physics/primitives/box.hpp"

Box::Box(vec2 size) : Collider()  {
    this->size = size;
    this->halfSize = size * 0.5f;
}

Box::~Box() {
    
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

void Box::setSize(vec2 size) {
    this->size = size;
    this->halfSize = size * 0.5f;
}