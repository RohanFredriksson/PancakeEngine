#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
#include <glm/glm.hpp>
#include "core/scene.hpp"

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

    // For each entity, check their new component list.
    // If there are components in this list, 
    // try and add them to the renderer and physics system.
    for (auto const& x : this->entities) {
        Entity* e = x.second;
        for (Component* c : e->getNewComponents()) {
            //this->renderer->add(c);
            //this->physics->add(c);
        }
        e->clearNewComponents();
    }

    /*
    for (Entity* e : this->entities) {
        for (Component* c : e->getNewComponents()) {
            //this->renderer->add(c);
            //this->physics->add(c);
        }
        e->clearNewComponents();
    }
    */
    
}

void Scene::update(float dt) {

    // Add all new components to their specific systems.
    this->addNewComponents();

    // Adjust the projection and step the physics engine.
    this->camera->adjustProjection();
    // this->physics->update(dt);

    // Update all the entities.
    deque<int> deadIds;
    for (auto const& x : this->entities) {
        Entity* e = x.second;
        
        // If the component is not dead, update it.
        if (e->isDead()) {
            e->update(dt);
        } 
        
        // If the component is dead, add its index a clear list.
        else {
            deadIds.push_front(e->getId());
        }

    }

    // Delete all dead elements
    for (int i : deadIds) {
        auto search = this->entities.find(i);
        Entity* e = search->second;
        delete e;
        this->entities.erase(i);
    }
    deadIds.clear();    

}

void Scene::render() {
    //this->renderer->render();
}