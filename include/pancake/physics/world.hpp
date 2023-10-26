#pragma once

#include <vector>
#include <string>
#include <utility>
#include <unordered_set>
#include <nlohmann/json.hpp>

#include "pancake/core/spatial.hpp"
#include "pancake/physics/force.hpp"
#include "pancake/physics/collision.hpp"
#include "pancake/physics/raycast.hpp"

namespace Pancake {

    class CollisionListener {
        public: virtual void collision(Entity* with, CollisionManifold manifold) {}
    };

    class World {

        private:

            ForceRegistry registry;
            std::vector<ForceGenerator*> forces;
            std::unordered_map<std::string, ForceGenerator*> forcesIndex;

            std::vector<Rigidbody*> rigidbodies;
            SpatialHashGrid<Rigidbody*>* grid;

            float timeStep;
            float time;

            void fixedUpdate();

        public:

            World(float timeStep, glm::vec2 gravity);
            ~World();

            void update(float dt);
            void render();
            nlohmann::json serialise();
            void load(nlohmann::json j);
            void add(Rigidbody* rigidbody);
            void remove(Rigidbody* rigidbody);
            RaycastResult raycast(Ray ray);

            ForceGenerator* getForceGenerator(std::string type);
            void addForceGenerator(ForceGenerator* force);
            void removeForceGenerator(ForceGenerator* force);
            void removeForceGenerator(std::string type);

            void addForceRegistration(std::string force, Rigidbody* rigidbody);
            void removeForceRegistration(std::string force, Rigidbody* rigidbody);

            SpatialHashGrid<Rigidbody*>* getGrid();

    };

}