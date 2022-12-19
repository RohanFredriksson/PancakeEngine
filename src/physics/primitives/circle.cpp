#include "physics/primitives/circle.hpp"

Circle::Circle() : Collider() {
    this->radius = 0.5f;
}

float Circle::getRadius() {
    return this->radius;
}

void Circle::setRadius(float radius) {
    this->radius = radius;
}