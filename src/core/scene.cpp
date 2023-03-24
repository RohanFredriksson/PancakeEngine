#include <string>
#include <vector>
#include <deque>
#include <glm/glm.hpp>
#include "pancake/core/scene.hpp"
#include "pancake/window/listener.hpp"

using std::deque;
using std::pair;
using glm::vec2;

#include <iostream>

Scene::Scene(string name, void (*init)(Scene* scene)) {

    this->name = name;
    this->camera = new Camera(vec2(0.0f, 0.0f), vec2(12.0f, 12.0f), 1.0f);
    this->renderer = new Renderer();
    this->physics = new World(1.0f / 60.0f, vec2(0.0f, -10.0f));
    if (init != NULL) {init(this);}

}

Scene::Scene(string name, string filename) {

    this->name = name;
    this->camera = new Camera(vec2(0.0f, 0.0f), vec2(12.0f, 12.0f), 1.0f);
    this->renderer = new Renderer();
    this->physics = new World(1.0f / 60.0f, vec2(0.0f, -10.0f));
    // Load scene from file

}

Scene::Scene(string name, string filename, void (*init)(Scene* scene)) {

    this->name = name;
    this->camera = new Camera(vec2(0.0f, 0.0f), vec2(12.0f, 12.0f), 1.0f);
    this->renderer = new Renderer();
    this->physics = new World(1.0f / 60.0f, vec2(0.0f, -10.0f));
    // Load scene from file
    if (init != NULL) {init(this);}

}

Scene::~Scene() {
    
    // Delete all scene elements.
    delete this->camera;
    delete this->renderer;
    delete this->physics;

    // Delete all entities and their components.
    for (auto const& x : this->entities) {
        Entity* e = x.second;
        delete e;
    }

}

void Scene::updateCallbackComponents() {

    for (auto const& x : this->entities) {
        Entity* e = x.second;
        for (Component* c : e->getCallbackUpdatedComponents()) {
            
            // Remove the component from all sets.
            this->keyDownComponents.erase(c->getId());
            this->mouseDownComponents.erase(c->getId());
            this->mouseDraggingComponents.erase(c->getId());

            // Add the component to the maps that it belongs to.
            if (c->hasKeyDownCallback())       {this->keyDownComponents[c->getId()] = c;}
            if (c->hasMouseDownCallback())      {this->mouseDownComponents[c->getId()] = c;}
            if (c->hasMouseDraggingCallback())  {this->mouseDraggingComponents[c->getId()] = c;}
            c->clearCallbackUpdate();

        }
        e->clearCallbackUpdatedComponents();
    }

}

void Scene::addNewComponents() {

    // For each entity, check their new component list.
    // If there are components in this list, 
    // try and add them to the renderer and physics system.
    for (auto const& x : this->entities) {
        Entity* e = x.second;
        for (Component* c : e->getNewComponents()) {
            
            // If the component is of the correct subclass, add it to its specific system.
            if (dynamic_cast<SpriteRenderer*>(c) != nullptr) {this->renderer->add((SpriteRenderer*) c);}
            if (dynamic_cast<Rigidbody*>(c) != nullptr) {this->physics->add((Rigidbody*) c);}

            // Add the component to the map for quick lookups.
            this->components[c->getId()] = c;
        }
        e->clearNewComponents();
    }
    
}

void Scene::removeDeadComponents() {

    // For each entity, check their dead component list.
    // If there are components in this list, delete them.
    for (auto const& x : this->entities) {
        Entity* e = x.second;
        for (int id : e->getDeadComponentIds()) {
            this->components.erase(id);
            this->keyDownComponents.erase(id);
            this->mouseDownComponents.erase(id);
            this->mouseDraggingComponents.erase(id);
        }
        e->clearDeadComponentIds();
    }

}

void Scene::update(float dt) {

    // Add all new components to their specific systems.
    this->addNewComponents(); 
    this->updateCallbackComponents();

    // Adjust the projection and step the physics engine.
    this->camera->adjustProjection();
    this->physics->update(dt); // This will update colliding components

    // Check for events.
    if (KeyListener::isKeyDown()) {for (const auto& pair : this->keyDownComponents) {
        pair.second->keyDownCallback();
    }}

    if (MouseListener::isMouseDown()) {for (const auto& pair : this->mouseDownComponents) {
        pair.second->mouseDownCallback();
    }}

    if (MouseListener::isMouseDragging()) {for (const auto& pair : this->mouseDraggingComponents) {
        pair.second->mouseDraggingCallback();
    }}

    // Update all the entities.
    deque<int> deadIds;
    for (auto const& x : this->entities) {
        Entity* e = x.second;

        // If the component is not dead, update it.
        if (!e->isDead()) {
            e->update(dt);
        } 
        
        // If the component is dead, add its index a clear list.
        else {
            deadIds.push_front(e->getId());
        }

    }

    // Delete all dead elements
    for (int i : deadIds) {
        
        // Find the entity using its id.
        auto search = this->entities.find(i);
        Entity* e = search->second;

        // Remove all components associated with the entity from the map.
        for (Component* c : e->getComponents()) {
            std::cout << "DELETED " << c->getId() << "\n";
            this->components.erase(c->getId());
            this->keyDownComponents.erase(c->getId());
            this->mouseDownComponents.erase(c->getId());
            this->mouseDraggingComponents.erase(c->getId());
        }

        // Delete the entity.
        delete e;
        this->entities.erase(i);

    }
    deadIds.clear();

    // Add all new components to their specific systems.
    this->addNewComponents(); 
    this->removeDeadComponents();

}

void Scene::render() {
    this->renderer->render();
    this->physics->render();
}

Camera* Scene::getCamera() {
    return this->camera;
}

Renderer* Scene::getRenderer() {
    return this->renderer;
}

World* Scene::getPhysics() {
    return this->physics;
}

void Scene::addEntity(Entity* entity) {
    pair<int, Entity*> p(entity->getId(), entity);
    this->entities.insert(p);
}

Entity* Scene::getEntityById(int id) {
    auto search = this->entities.find(id);
    if (search == this->entities.end()) {return NULL;}
    Entity* e = search->second;
    return e;
}

Component* Scene::getComponentById(int id) {
    auto search = this->components.find(id);
    if (search == this->components.end()) {return NULL;}
    Component* c = search->second;
    return c;
}