#include "physics/primitives/circle.hpp"

Circle::Circle(float radius) : Collider() {
    this->radius = radius;
}

float Circle::getRadius() {
    return this->radius;
}

void Circle::setRadius(float radius) {
    this->radius = radius;
}