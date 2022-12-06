#pragma once

#include <vector>
#include "physics/forces/forceregistry.hpp"
#include "physics/collision/collision.hpp"
#include "physics/forces/gravity.hpp"

using std::vector;

class World {

    private:

        ForceRegistry registry;
        Gravity* gravity;
        vector<Rigidbody*> rigidbodies;
        vector<Rigidbody*> bodies1;
        vector<Rigidbody*> bodies2;
        vector<CollisionManifold> collisions;
        float timeStep;
        float time;

        void fixedUpdate();
        void clearCollisionLists();

    public:

        World(float timeStep, vec2 gravity);
        ~World();

        void update(float dt);
        void render();
        void add(Rigidbody* rigidbody);
        void remove(Rigidbody* rigidbody);

};