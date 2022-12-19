#include "physics/primitives/box.hpp"

Box::Box() : Collider()  {
    this->size = vec2(1.0f, 1.0f);
    this->halfSize = this->size * 0.5f;
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