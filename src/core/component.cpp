#include "core/entity.hpp"

Component::Component() {
    
}

Component::~Component() {

}

void Component::update(float dt) {

}

void Component::kill() {
    this->dead = true;
}

bool Component::isDead() {
    return this->dead;
}