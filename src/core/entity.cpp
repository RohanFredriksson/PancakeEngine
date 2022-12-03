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

bool Entity::isDead() {
    return this->dead;
}