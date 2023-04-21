#include "pancake/physics/circle.hpp"

namespace Pancake {

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