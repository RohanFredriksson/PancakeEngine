#pragma once

#include <vector>
#include <nlohmann/json.hpp>

#include "pancake/physics/forces/forceregistry.hpp"
#include "pancake/physics/collision/collision.hpp"
#include "pancake/physics/forces/gravity.hpp"
#include "pancake/physics/raycast/raycast.hpp"

using std::vector;
using json = nlohmann::json;

class World {

    private:

        ForceRegistry registry;
        Gravity* gravity;
        vector<Rigidbody*> rigidbodies;
        vector<Rigidbody*> bodies1;
        vector<Rigidbody*> bodies2;
        vector<vector<CollisionManifold*>> collisions;
        float timeStep;
        float time;

        void fixedUpdate();
        void clearCollisionLists();

    public:

        World(float timeStep, vec2 gravity);
        ~World();

        void update(float dt);
        void render();
        json serialise();
        void load(json j);
        void add(Rigidbody* rigidbody);
        void remove(Rigidbody* rigidbody);
        RaycastResult raycast(Ray* ray);

        vec2 getGravity();
        void setGravity(vec2 gravity);

};