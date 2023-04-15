#include <deque>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

#include "pancake/core/scene.hpp"
#include "pancake/core/listener.hpp"
#include "pancake/asset/assetpool.hpp"

using std::deque;
using std::pair;
using glm::vec2;

Scene::Scene(string name, string filename, void (*init)(Scene* scene)) {

    this->name = name;
    this->started = false;
    this->camera = new Camera(vec2(0.0f, 0.0f), vec2(12.0f, 12.0f), 1.0f);
    this->renderer = new Renderer();
    this->physics = new World(1.0f / 60.0f, vec2(0.0f, -10.0f));
    if (!filename.empty()) {this->load(filename);}
    if (init != nullptr) {init(this);}

    this->start();

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
        }
        e->clearDeadComponentIds();
    }

}

void Scene::start() {
    for (auto const& x : this->entities) {
        Entity* e = x.second;
        e->start();
    }
    this->started = true;
}

void Scene::update(float dt) {

    // Add all new components to their specific systems.
    this->addNewComponents(); 

    // Adjust the projection and step the physics engine.
    this->camera->adjustProjection();
    this->camera->update(dt);
    this->physics->update(dt); // This will update colliding components

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
    j.emplace("physics", this->physics->serialise());
    j.emplace("fonts", FontPool::serialise());
    j.emplace("sprites", SpritePool::serialise());
    j.emplace("audio", AudioPool::serialise());

    j.emplace("entities", json::array());
    for (auto const& x : this->entities) {
        Entity* e = x.second;
        if (e->isSerialisable() && !e->isDead()) {j["entities"].push_back(e->serialise());}
    }

    return j;

}

void Scene::save(string filename) {

    string contents = this->serialise().dump(4);
    std::ofstream file(filename);
    file << contents;
    file.close();

}

void Scene::load(string filename) {

    std::ifstream f(filename);
    json j;

    try {
        std::ifstream f(filename);
        j = json::parse(f);
    } 
    
    catch (const std::ifstream::failure& e) {
        std::cout << "ERROR::SCENE::LOAD::FILE_NOT_FOUND\n";
        return;
    }

    catch (const json::exception& e) {
        std::cout << "ERROR::SCENE::LOAD::JSON_PARSE_ERROR\n";
        return;
    }

    // Load camera settings into the camera.
    if (j.contains("camera") && j["camera"].is_object()) {
        this->camera->load(j["camera"]);
    }

    // Load physics configuration into the engine.
    if (j.contains("physics") && j["physics"].is_object()) {
        this->physics->load(j["physics"]);
    }

    // Load fonts into the font pool
    if (j.contains("fonts") && j["fonts"].is_array()) {
        for (auto element : j["fonts"]) {
            if (element.is_object()) {
                Font::load(element);
            }
        }
    }

    // Load audio into the audio pool
    if (j.contains("audio") && j["audio"].is_array()) {
        for (auto element : j["fonts"]) {
            if (element.is_object()) {
                AudioWave::load(element);
            }
        }
    }

    // Load sprites into the sprite pool.
    if (j.contains("sprites") && j["sprites"].is_array()) {
        for (auto element : j["sprites"]) {
            if (element.is_object()) {
                Sprite::load(element); 
            }
        }
    }

    // Create new entities and add them to the scene.
    if (j.contains("entities") && j["entities"].is_array()) {
        for (auto element : j["entities"]) {
            if (element.is_object()) {
                Entity* e = Entity::load(element);
                if (e != nullptr) {this->addEntity(e);}
            }
        }
    }

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
    if (this->started) {entity->start();}
}

Entity* Scene::getEntityById(int id) {
    auto search = this->entities.find(id);
    if (search == this->entities.end()) {return nullptr;}
    Entity* e = search->second;
    return e;
}

Component* Scene::getComponentById(int id) {
    auto search = this->components.find(id);
    if (search == this->components.end()) {return nullptr;}
    Component* c = search->second;
    return c;
}