#include <deque>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

#include "pancake/core/scene.hpp"
#include "pancake/core/listener.hpp"
#include "pancake/physics/rigidbody.hpp"
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
        this->updatingTimers = false;
        this->nextTimerId = 1;
    }

    Scene::~Scene() {
        
        // Kill all entities, so components unregister while the scene systems still exist.
        for (Entity* e : this->entities) {
            e->kill();
        }

        // Delete all entities and their components.
        for (Entity* e : this->entities) {
            delete e;
        }

        this->gridBounds.clear();

        // Delete all scene elements.
        delete this->camera;
        delete this->renderer;
        delete this->physics;
        delete this->grid;

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
        this->physics->render();   // Queue physics debug lines, if enabled.

        // Update all the entities.
        std::deque<int> dead;
        for (int i = 0; i < this->entities.size(); i++) {
            if (!this->entities[i]->isDead()) {this->entities[i]->update(dt);} 
            else {dead.push_front(i);}
        }

        // Run timers after entities, so a timer owned by an entity killed this frame doesn't fire.
        this->updateTimers(dt);

        // Delete all dead elements
        for (int i : dead) {
            Entity* e = this->entities[i];
            this->entityIndex.erase(e->getId());
            this->entities.erase(this->entities.begin() + i);
            this->grid->remove(e);
            this->gridBounds.erase(e);
            delete e;
        }

        // Keep the spatial hash grid in sync with entities that have moved.
        for (Entity* e : this->entities) {
            if (!e->isDead()) {this->updateGrid(e);}
        }

    }

    void Scene::updateTimers(float dt) {

        this->updatingTimers = true;

        // Callbacks can cancel timers but not add to this vector, so references into it stay valid.
        for (Timer& t : this->timers) {
            if (t.cancelled) {continue;}
            if (t.owner >= 0) {
                Entity* owner = this->getEntity(t.owner);
                if (owner == nullptr || owner->isDead()) {t.cancelled = true; continue;}
            }
            t.remaining -= dt;
            if (t.remaining > 0.0f) {continue;}
            if (!t.repeat) {t.cancelled = true;}
            else {t.remaining = std::max(t.remaining + t.interval, 0.0f);} // Fires at most once a frame.
            t.callback();
        }

        this->updatingTimers = false;
        this->timers.erase(std::remove_if(this->timers.begin(), this->timers.end(), [](const Timer& t) {return t.cancelled;}), this->timers.end());
        for (Timer& t : this->addedTimers) {
            if (!t.cancelled) {this->timers.push_back(std::move(t));}
        }
        this->addedTimers.clear();

    }

    int Scene::addTimer(float seconds, bool repeat, std::function<void()> callback, Entity* owner) {
        Timer t;
        t.id = this->nextTimerId++;
        t.remaining = seconds;
        t.interval = std::max(seconds, 0.0f);
        t.repeat = repeat;
        t.owner = owner == nullptr ? -1 : owner->getId();
        t.cancelled = false;
        t.callback = callback;
        if (this->updatingTimers) {this->addedTimers.push_back(std::move(t));}
        else {this->timers.push_back(std::move(t));}
        return this->nextTimerId - 1;
    }

    int Scene::after(float seconds, std::function<void()> callback, Entity* owner) {
        return this->addTimer(seconds, false, callback, owner);
    }

    int Scene::every(float seconds, std::function<void()> callback, Entity* owner) {
        return this->addTimer(seconds, true, callback, owner);
    }

    void Scene::cancel(int timer) {
        for (Timer& t : this->timers) {if (t.id == timer) {t.cancelled = true;}}
        for (Timer& t : this->addedTimers) {if (t.id == timer) {t.cancelled = true;}}
    }

    bool Scene::hasTimer(int timer) {
        for (Timer& t : this->timers) {if (t.id == timer && !t.cancelled) {return true;}}
        for (Timer& t : this->addedTimers) {if (t.id == timer && !t.cancelled) {return true;}}
        return false;
    }

    void Scene::render() {
        this->renderer->render();
    }

    nlohmann::json Scene::serialise() {

        nlohmann::json j;
        j.emplace("name", this->name);
        j.emplace("camera", this->camera->serialise());
        j.emplace("gravity", nlohmann::json::array({this->physics->getGravity().x, this->physics->getGravity().y}));
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
        if (!f.is_open()) {
            std::cout << "ERROR::SCENE::LOAD::FILE_NOT_FOUND\n";
            return;
        }

        nlohmann::json j;
        try {
            j = nlohmann::json::parse(f);
        } 

        catch (const nlohmann::json::exception& e) {
            std::cout << "ERROR::SCENE::LOAD::JSON_PARSE_ERROR\n";
            return;
        }

        // Load the scene name.
        if (j.contains("name") && j["name"].is_string()) {
            this->name = j["name"];
        }

        // Load the world gravity.
        if (j.contains("gravity") && j["gravity"].is_array() && j["gravity"].size() == 2 && j["gravity"][0].is_number() && j["gravity"][1].is_number()) {
            this->physics->setGravity(glm::vec2(j["gravity"][0], j["gravity"][1]));
        }

        // Load camera settings into the camera.
        if (j.contains("camera") && j["camera"].is_object()) {
            this->camera->load(j["camera"]);
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
            for (auto element : j["audio"]) {
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
        this->updateGrid(entity);
    }

    std::vector<Entity*> Scene::getEntities() {
        return this->entities;
    }

    Entity* Scene::findEntity(std::string name) {
        for (Entity* e : this->entities) {
            if (!e->isDead() && e->getName() == name) {return e;}
        }
        return nullptr;
    }

    std::vector<Entity*> Scene::findEntities(std::string name) {
        std::vector<Entity*> result;
        for (Entity* e : this->entities) {
            if (!e->isDead() && e->getName() == name) {result.push_back(e);}
        }
        return result;
    }

    std::pair<glm::vec2, glm::vec2> Scene::computeBounds(Entity* entity) {

        // The entity's transform, as an axis aligned box around its rotated size.
        glm::vec2 size = glm::abs(entity->getSize());
        float c = std::abs(cosf(entity->getRotation()));
        float s = std::abs(sinf(entity->getRotation()));
        glm::vec2 half = 0.5f * glm::vec2(size.x * c + size.y * s, size.x * s + size.y * c);
        glm::vec2 min = entity->getPosition() - half;
        glm::vec2 max = entity->getPosition() + half;

        // Grow the box to cover the rigidbody's colliders.
        Rigidbody* rigidbody = dynamic_cast<Rigidbody*>(entity->getComponent("Rigidbody"));
        if (rigidbody != nullptr && !rigidbody->getColliders().empty()) {
            std::pair<glm::vec2, glm::vec2> bounds = rigidbody->getBounds();
            min = glm::min(min, bounds.first);
            max = glm::max(max, bounds.second);
        }

        return std::make_pair(min, max);

    }

    void Scene::updateGrid(Entity* entity) {

        std::pair<glm::vec2, glm::vec2> bounds = this->computeBounds(entity);
        auto it = this->gridBounds.find(entity);
        if (it != this->gridBounds.end() && it->second == bounds) {return;}

        this->gridBounds[entity] = bounds;
        this->grid->update(entity, bounds.first, bounds.second);

    }

    std::pair<glm::vec2, glm::vec2> Scene::getBounds(Entity* entity) {
        auto it = this->gridBounds.find(entity);
        if (it != this->gridBounds.end()) {return it->second;}
        return this->computeBounds(entity);
    }

    std::vector<Entity*> Scene::getEntitiesIn(glm::vec2 min, glm::vec2 max) {

        std::vector<Entity*> result;
        glm::vec2 centre = 0.5f * (min + max);
        glm::vec2 size = max - min;

        for (Entity* e : this->grid->get(centre.x, centre.y, size.x, size.y)) {
            if (e->isDead()) {continue;}
            std::pair<glm::vec2, glm::vec2> b = this->getBounds(e);
            bool overlaps = b.first.x <= max.x && b.second.x >= min.x && b.first.y <= max.y && b.second.y >= min.y;
            if (overlaps) {result.push_back(e);}
        }

        return result;

    }

    std::vector<Entity*> Scene::getEntitiesNear(glm::vec2 position, float radius) {

        std::vector<Entity*> result;
        glm::vec2 extent = glm::vec2(radius, radius);

        for (Entity* e : this->getEntitiesIn(position - extent, position + extent)) {
            std::pair<glm::vec2, glm::vec2> b = this->getBounds(e);
            glm::vec2 closest = glm::clamp(position, b.first, b.second);
            if (glm::distance(closest, position) <= radius) {result.push_back(e);}
        }

        return result;

    }

    std::vector<Entity*> Scene::getEntitiesAt(glm::vec2 position) {
        return this->getEntitiesIn(position, position);
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