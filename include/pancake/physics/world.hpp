#pragma once

#include <vector>
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
            Gravity* gravity;
            std::vector<Rigidbody*> rigidbodies;
            std::vector<Rigidbody*> bodies1;
            std::vector<Rigidbody*> bodies2;
            std::vector<std::vector<CollisionManifold>> collisions;
            float timeStep;
            float time;

            void fixedUpdate();
            void clearCollisionLists();

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

            glm::vec2 getGravity();
            void setGravity(glm::vec2 gravity);

    };

}