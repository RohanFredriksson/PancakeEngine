#include "pancake/physics/primitives/circle.hpp"

Circle::Circle() : Collider() {
    this->radius = 0.5f;
}

json Circle::serialise() {
    json j = this->Collider::serialise();
    j.emplace("radius", this->radius);
    return j;
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