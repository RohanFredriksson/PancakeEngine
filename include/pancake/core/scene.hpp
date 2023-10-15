#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "pancake/core/camera.hpp"
#include "pancake/core/entity.hpp"
#include "pancake/core/component.hpp"
#include "pancake/core/spatial.hpp"
#include "pancake/graphics/renderer.hpp"
#include "pancake/physics/world.hpp"

namespace Pancake {

    class Scene {

        private:

            std::string name;
            std::vector<Entity*> entities;
            std::unordered_map<int, Entity*> entityIndex;
            std::unordered_map<int, Component*> componentIndex;
            
            bool started;
            
            Camera* camera;
            Renderer* renderer;
            World* physics;
            SpatialHashGrid<Entity*>* grid;

        public:

            Scene();
            Scene(std::string name);
            ~Scene();

            void start();
            void update(float dt);
            void render();
            nlohmann::json serialise();
            void save(std::string filename);
            void load(std::string filename);

            std::string getName();
            Camera* getCamera();
            Renderer* getRenderer();
            World* getPhysics();

            void setName(std::string name);
            
            void addEntity(Entity* entity);
            Entity* getEntity(int id);
            Component* getComponent(int id);
            std::unordered_map<int, Component*>* getComponents();

    };

}