#pragma once

#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
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

    // The impulses applied at a contact point, kept between steps to warm start the solver.
    struct CachedContact {
        glm::vec2 point;
        glm::vec2 normal;
        float normalImpulse;
        float tangentImpulse;
    };

    class World {

        private:

            std::unordered_map<std::pair<int, int>, std::vector<CachedContact>, IntPairHash, IntPairEqual> contactCache;

            ForceRegistry registry;
            
            std::vector<ForceGenerator*> forces;
            std::unordered_map<std::string, ForceGenerator*> forcesIndex;

            std::vector<Rigidbody*> rigidbodies;
            std::unordered_set<Rigidbody*> rigidbodiesIndex;

            SpatialHashGrid<Rigidbody*>* grid;

            glm::vec2 gravity;
            float timeStep;
            float time;
            bool debug;

            void fixedUpdate();

        public:

            World(float timeStep, glm::vec2 gravity);
            ~World();

            void update(float dt);
            void render();
            glm::vec2 getGravity();
            void setGravity(glm::vec2 gravity);
            bool hasDebugDraw();
            void setDebugDraw(bool debug);
            void add(Rigidbody* rigidbody);
            bool has(Rigidbody* rigidbody);
            void remove(Rigidbody* rigidbody);
            
            ForceGenerator* getForceGenerator(std::string type);
            void addForceGenerator(ForceGenerator* force);
            void removeForceGenerator(ForceGenerator* force);
            void removeForceGenerator(std::string type);

            void addForceRegistration(std::string force, Rigidbody* rigidbody);
            void removeForceRegistration(std::string force, Rigidbody* rigidbody);

            SpatialHashGrid<Rigidbody*>* getGrid();
            RaycastResult raycast(Ray ray);
            RaycastResult raycast(Ray ray, float maxDistance);

    };

}