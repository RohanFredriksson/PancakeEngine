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
            std::unordered_map<Entity*, std::pair<glm::vec2, glm::vec2>> gridBounds;

            std::pair<glm::vec2, glm::vec2> computeBounds(Entity* entity);
            void updateGrid(Entity* entity);

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
            bool hasStarted();

            void setName(std::string name);
            
            void addEntity(Entity* entity);
            Entity* getEntity(int id);
            Component* getComponent(int id);
            std::unordered_map<int, Component*>* getComponents();
            std::vector<Entity*> getEntities();

            // Lookups by name or component. Dead entities are skipped.
            Entity* findEntity(std::string name);
            std::vector<Entity*> findEntities(std::string name);
            template<class T> std::vector<Entity*> getEntitiesWith();

            // Spatial queries. An entity's bounds cover its transform and its rigidbody, if it has one.
            std::pair<glm::vec2, glm::vec2> getBounds(Entity* entity);
            std::vector<Entity*> getEntitiesIn(glm::vec2 min, glm::vec2 max);
            std::vector<Entity*> getEntitiesNear(glm::vec2 position, float radius);
            std::vector<Entity*> getEntitiesAt(glm::vec2 position);

    };

}

namespace Pancake {

    template<class T>
    std::vector<Entity*> Scene::getEntitiesWith() {
        std::vector<Entity*> result;
        for (Entity* e : this->entities) {
            if (!e->isDead() && e->hasComponent<T>()) {result.push_back(e);}
        }
        return result;
    }

}
