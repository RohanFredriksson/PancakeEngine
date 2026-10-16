#pragma once

#include <vector>
#include <string>
#include <utility>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <nlohmann/json.hpp>

#include "pancake/core/spatial.hpp"
#include "pancake/physics/force.hpp"
#include "pancake/physics/collision.hpp"
#include "pancake/physics/raycast.hpp"

namespace Pancake {

    // Components that implement this are told about their entity's collisions. Calls are made at the end of
    // each physics step, so listeners can kill entities or change bodies safely. Sensors get them too.
    class CollisionListener {
        public:
            virtual void collision(Entity* with, CollisionManifold manifold) {}     // Every contact, every step.
            virtual void collisionEnter(Entity* with, CollisionManifold manifold) {} // The first step two bodies touch.
            virtual void collisionExit(Entity* with) {}                             // The first step they don't, or one leaves the world.
    };

    const uint32_t ALL_LAYERS = 0xFFFFFFFF;

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

            // Body pairs touching at the end of the last step, keyed by ordered ids, to find enter and exit events.
            std::unordered_map<std::pair<int, int>, std::pair<Rigidbody*, Rigidbody*>, IntPairHash, IntPairEqual> touching;

            void fixedUpdate();

        public:

            World(float timeStep, glm::vec2 gravity);
            ~World();

            void update(float dt);
            void render();
            float getTimeStep();
            glm::vec2 getGravity();
            void setGravity(glm::vec2 gravity);
            bool hasDebugDraw();
            void setDebugDraw(bool debug);
            void add(Rigidbody* rigidbody);
            bool has(Rigidbody* rigidbody);
            void remove(Rigidbody* rigidbody);
            bool isTouching(Rigidbody* a, Rigidbody* b);
            void clearContacts(); // Forget contacts without exit events, e.g. when the whole scene is going away.
            
            ForceGenerator* getForceGenerator(std::string type);
            void addForceGenerator(ForceGenerator* force);
            void removeForceGenerator(ForceGenerator* force);
            void removeForceGenerator(std::string type);

            void addForceRegistration(std::string force, Rigidbody* rigidbody);
            void removeForceRegistration(std::string force, Rigidbody* rigidbody);

            SpatialHashGrid<Rigidbody*>* getGrid();
            // Only bodies whose collision layer is in the mask are hit.
            RaycastResult raycast(Ray ray);
            RaycastResult raycast(Ray ray, float maxDistance, uint32_t mask = ALL_LAYERS);

    };

}