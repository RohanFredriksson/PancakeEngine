#include <deque>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

#include "pancake/core/scene.hpp"
#include "pancake/core/listener.hpp"
#include "pancake/asset/assetpool.hpp"
#include "pancake/asset/spritesheet.hpp"

using std::deque;
using std::pair;
using glm::vec2;

namespace Pancake {

    Scene::Scene() : Scene("New Scene") {

    }

    Scene::Scene(string name) {
        this->name = name;
        this->started = false;
        this->camera = new Camera(vec2(0.0f, 0.0f), vec2(12.0f, 12.0f), 1.0f);
        this->renderer = new Renderer();
        this->physics = new World(1.0f / 60.0f, vec2(0.0f, -10.0f));
    }

    Scene::~Scene() {
        
        // Kill all entities.
        for (auto const& x: this->entities) {
            Entity* e = x.second;
            e->kill();
        }

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

    void Scene::start() {
        this->started = true;
        for (auto const& x : this->entities) {
            Entity* e = x.second;
            e->start();
        }
    }

    void Scene::update(float dt) {

        // Adjust the projection and step the physics engine.
        this->camera->adjustProjection();
        this->camera->update(dt);
        this->physics->update(dt); // This will update colliding components

        // Update all the entities.
        deque<int> dead;
        for (auto const& x : this->entities) { // TODO: THIS MAY CRASH THE ENGINE. ADDITION OF NEW ENTITIES IN THE LOOP MODIFIES THE ENTITY HASHMAP.
            Entity* e = x.second;
            if (!e->isDead()) {e->update(dt);} // If the component is not dead, update it.
            else {dead.push_front(e->getId());} // If the component is dead, add its index a clear list.
        }

        // Delete all dead elements
        for (int i : dead) {
            auto search = this->entities.find(i); // Find the entity using its id.
            Entity* e = search->second;
            delete e; // Delete the entity.
            this->entities.erase(i);
        }

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
        j.emplace("spritesheets", Spritesheet::serialise());
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

        // Load sprites from spritesheets into the sprite pool.
        if (j.contains("spritesheets") && j["spritesheets"].is_array()) {
            for (auto element : j["spritesheets"]) {
                if (element.is_string()) {
                    Spritesheet::load(element);
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

    string Scene::getName() {
        return this->name;
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

    void Scene::setName(string name) {
        this->name = name;
    }

    void Scene::addEntity(Entity* entity) {
        pair<int, Entity*> p(entity->getId(), entity);
        this->entities.insert(p);
        if (this->started) {entity->start();}
    }

    Entity* Scene::getEntity(int id) {
        auto search = this->entities.find(id);
        if (search == this->entities.end()) {return nullptr;}
        Entity* e = search->second;
        return e;
    }

    Component* Scene::getComponent(int id) {
        auto search = this->components.find(id);
        if (search == this->components.end()) {return nullptr;}
        Component* c = search->second;
        return c;
    }

    unordered_map<int, Entity*>* Scene::getEntities() {
        return &this->entities;
    }

    unordered_map<int, Component*>* Scene::getComponents() {
        return &this->components;
    }

}