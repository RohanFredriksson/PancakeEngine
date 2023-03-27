#include <string>
#include <vector>
#include <deque>
#include <glm/glm.hpp>

#include "pancake/core/scene.hpp"
#include "pancake/window/listener.hpp"
#include "pancake/util/assetpool.hpp"

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

            // If the component has an event register it to the event system.
            if (dynamic_cast<KeyDown*>(c) != nullptr) {this->keyDownComponents[c->getId()] = c;}
            if (dynamic_cast<MouseDown*>(c) != nullptr) {this->mouseDownComponents[c->getId()] = c;}
            if (dynamic_cast<MouseDragging*>(c) != nullptr) {this->mouseDraggingComponents[c->getId()] = c;}
            
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

    // Adjust the projection and step the physics engine.
    this->camera->adjustProjection();
    this->camera->update(dt);
    this->physics->update(dt); // This will update colliding components

    // Check for events.
    if (KeyListener::isKeyDown()) {for (const auto& pair : this->keyDownComponents) {
        dynamic_cast<KeyDown*>(pair.second)->keyDownCallback();
    }}

    if (MouseListener::isMouseDown()) {for (const auto& pair : this->mouseDownComponents) {
        dynamic_cast<MouseDown*>(pair.second)->mouseDownCallback();
    }}

    if (MouseListener::isMouseDragging()) {for (const auto& pair : this->mouseDraggingComponents) {
        dynamic_cast<MouseDragging*>(pair.second)->mouseDraggingCallback();
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

json Scene::serialise() {

    json j;
    j.emplace("name", this->name);
    j.emplace("camera", this->camera->serialise());
    
    j.emplace("gravity", json::array());
    j["gravity"].push_back(this->physics->getGravity().x);
    j["gravity"].push_back(this->physics->getGravity().y);

    j.emplace("fonts", FontPool::serialise());
    j.emplace("sprites", SpritePool::serialise());

    j.emplace("entities", json::array());
    for (auto const& x : this->entities) {
        Entity* e = x.second;
        j["entities"].push_back(e->serialise());
    }

    return j;

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