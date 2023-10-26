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

namespace Pancake {

    Scene::Scene() : Scene("New Scene") {

    }

    Scene::Scene(std::string name) {
        this->name = name;
        this->started = false;
        this->camera = new Camera(glm::vec2(0.0f, 0.0f), glm::vec2(12.0f, 12.0f), 1.0f);
        this->renderer = new Renderer();
        this->physics = new World(1.0f / 60.0f, glm::vec2(0.0f, -10.0f));
        this->grid = new SpatialHashGrid<Entity*>(4);
    }

    Scene::~Scene() {
        
        // Kill all entities.
        for (Entity* e : this->entities) {
            e->kill();
        }

        // Delete all scene elements.
        delete this->camera;
        delete this->renderer;
        delete this->physics;
        delete this->grid;

        // Delete all entities and their components.
        for (Entity* e : this->entities) {
            delete e;
        }

    }

    void Scene::start() {
        this->started = true;
        int n = this->entities.size();
        for (int i = 0; i < n; i++) {
            this->entities[i]->start();
        }
    }

    void Scene::update(float dt) {

        // Adjust the projection and step the physics engine.
        this->camera->adjustProjection();
        this->camera->update(dt);
        this->physics->update(dt); // This will update colliding components

        // Update all the entities.
        std::deque<int> dead;
        for (int i = 0; i < this->entities.size(); i++) {
            if (!this->entities[i]->isDead()) {this->entities[i]->update(dt);} 
            else {dead.push_front(i);}
        }

        // Delete all dead elements
        for (int i : dead) {
            Entity* e = this->entities[i];
            this->entityIndex.erase(e->getId());
            this->entities.erase(this->entities.begin() + i);
            delete e;
        }

    }

    void Scene::render() {
        this->renderer->render();
        this->physics->render();
    }

    nlohmann::json Scene::serialise() {

        nlohmann::json j;
        j.emplace("name", this->name);
        j.emplace("camera", this->camera->serialise());
        j.emplace("physics", this->physics->serialise());
        j.emplace("fonts", FontPool::serialise());
        j.emplace("spritesheets", Spritesheet::serialise());
        j.emplace("sprites", SpritePool::serialise());
        j.emplace("audio", AudioPool::serialise());

        j.emplace("entities", nlohmann::json::array());
        for (Entity* e : this->entities) {
            if (e->isSerialisable() && !e->isDead()) {j["entities"].push_back(e->serialise());}
        }

        return j;

    }

    void Scene::save(std::string filename) {

        std::string contents = this->serialise().dump(4);
        std::ofstream file(filename);
        file << contents;
        file.close();

    }

    void Scene::load(std::string filename) {

        std::ifstream f(filename);
        nlohmann::json j;

        try {
            std::ifstream f(filename);
            j = nlohmann::json::parse(f);
        } 
        
        catch (const std::ifstream::failure& e) {
            std::cout << "ERROR::SCENE::LOAD::FILE_NOT_FOUND\n";
            return;
        }

        catch (const nlohmann::json::exception& e) {
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

    std::string Scene::getName() {
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

    bool Scene::hasStarted() {
        return this->started;
    }

    void Scene::setName(std::string name) {
        this->name = name;
    }

    void Scene::addEntity(Entity* entity) {
        this->entities.push_back(entity);
        std::pair<int, Entity*> p(entity->getId(), entity);
        this->entityIndex.insert(p);
        if (this->started) {entity->start();}
    }

    Entity* Scene::getEntity(int id) {
        auto search = this->entityIndex.find(id);
        if (search == this->entityIndex.end()) {return nullptr;}
        Entity* e = search->second;
        return e;
    }

    Component* Scene::getComponent(int id) {
        auto search = this->componentIndex.find(id);
        if (search == this->componentIndex.end()) {return nullptr;}
        Component* c = search->second;
        return c;
    }

    std::unordered_map<int, Component*>* Scene::getComponents() {
        return &this->componentIndex;
    }

}