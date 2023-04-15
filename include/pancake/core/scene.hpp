#pragma once

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "pancake/core/camera.hpp"
#include "pancake/core/entity.hpp"
#include "pancake/core/component.hpp"
#include "pancake/graphics/renderer.hpp"
#include "pancake/physics/world.hpp"

using std::string;
using std::unordered_map;
using json = nlohmann::json;

class Scene {

    private:

        string name;
        unordered_map<int, Entity*> entities;
        unordered_map<int, Component*> components;
        
        Camera* camera;
        Renderer* renderer;
        World* physics;

        void addNewComponents();
        void removeDeadComponents();

    public:

        Scene(string name, string filename, void (*init)(Scene* scene));
        ~Scene();

        void update(float dt);
        void render();
        json serialise();
        void save(string filename);
        void load(string filename);

        Camera* getCamera();
        Renderer* getRenderer();
        World* getPhysics();

        void addEntity(Entity* entity);
        Entity* getEntityById(int id);
        Component* getComponentById(int id);

};