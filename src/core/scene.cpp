#include "core/scene.hpp"
#include <string>
#include <vector>
#include <deque>
#include <glm/glm.hpp>

using std::deque;
using glm::vec2;

Scene::Scene(string name) {
    this->name = name;
    this->camera = new Camera(vec2(0.0f, 0.0f), vec2(12.0f, 12.0f), 1.0f);
}

Scene::~Scene() {
    delete this->camera;
}

void Scene::addNewComponents() {

    for (Entity* e : this->entities) {
        for (Component* c : e->getNewComponents()) {
            //this->renderer->add(c);
            //this->physics->add(c);
        }
        e->clearNewComponents();
    }
    
}

void Scene::update(float dt) {

    // Add all new components to their specific systems.
    this->addNewComponents();

    // Adjust the projection and step the physics engine.
    this->camera->adjustProjection();
    // this->physics->update(dt);

    // Update all the entities.
    deque<int> deadIndices;
    int index = 0;
    for (Entity* e : this->entities) {
        
        // If the component is not dead, update it.
        if (e->isDead()) {
            e->update(dt);
        } 
        
        // If the component is dead, add its index a clear list.
        else {
            deadIndices.push_front(index);
        }

        index++;
    }

    // Delete all dead elements
    for (int i : deadIndices) {
        Entity* e = this->entities[i];
        delete e;
        this->entities.erase(this->entities.begin() + i);
    }
    deadIndices.clear();    

}

void Scene::render() {
    //this->renderer->render();
}