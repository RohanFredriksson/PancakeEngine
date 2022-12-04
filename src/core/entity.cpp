#include <deque>
#include "core/entity.hpp"

using std::deque;

Entity::Entity(vec2 position, vec2 size, float rotation) {
    this->id = -1;
    this->position = position;
    this->size = size;
    this->rotation = rotation;
    this->dead = false;
}

Entity::~Entity() {

    // Delete all the components.
    for (Component* c: this->components) {
        delete c;
    }

}

void Entity::update(float dt) {

    // Update all the components.
    deque<int> deadIndices;
    int index = 0;
    for (Component* c : this->components) {
        
        // If the component is not dead, update it.
        if (c->isDead()) {
            c->update(dt);
        } 
        
        // If the component is dead, add its index a clear list.
        else {
            deadIndices.push_front(index);
        }

        index++;
    }

    // Delete all dead elements
    for (int i : deadIndices) {
        Component* c = this->components[i];
        delete c;
        this->components.erase(this->components.begin() + i);
    }
    deadIndices.clear();

}

void Entity::kill() {
    this->dead = true;
}

int Entity::getId() {
    return this->id;
}

vector<Component*> Entity::getComponents() {
    return this->components;
}

vector<Component*> Entity::getNewComponents() {
    return this->newComponents;
}

vec2 Entity::getPosition() {
    return this->position;
}

vec2 Entity::getSize() {
    return this->size;
}

float Entity::getRotation() {
    return this->rotation;
}

bool Entity::isDead() {
    return this->dead;
}

void Entity::setPosition(vec2 position) {
    this->position = position;
}

void Entity::setSize(vec2 size) {
    this->size = size;
}

void Entity::setRotation(float rotation) {
    this->rotation = rotation;
}

void Entity::addPosition(vec2 position) {
    this->position += position;
}

void Entity::addSize(vec2 size) {
    this->size += size;
}

void Entity::addRotation(float rotation) {
    this->rotation += rotation;
}

void Entity::clearNewComponents() {
    this->newComponents.clear();
}