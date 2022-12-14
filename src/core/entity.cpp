#include <deque>
#include "core/entity.hpp"
#include "core/component.hpp"

using std::deque;

namespace {
    int nextId = 0;
}

Entity::Entity(vec2 position, vec2 size, float rotation) {
    
    this->id = nextId;
    this->position = position;
    this->size = size;
    this->rotation = rotation;
    this->dead = false;

    nextId++;
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
        if (!c->isDead()) {
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
        this->deadComponentIds.push_back(c->getId());
        this->components.erase(this->components.begin() + i);
        delete c;
    }
    deadIndices.clear();

}

void Entity::onCollision(Component* with) {
    for (Component* c : this->components) {
        c->onCollision(with);
    }
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

vector<int> Entity::getDeadComponentIds() {
    return this->deadComponentIds;
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

void Entity::addComponent(Component* component) {
    component->setEntity(this);
    this->components.push_back(component);
    this->newComponents.push_back(component);
}

void Entity::clearNewComponents() {
    this->newComponents.clear();
}

void Entity::clearDeadComponentIds() {
    this->deadComponentIds.clear();
}