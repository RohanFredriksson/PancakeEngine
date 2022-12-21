#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "core/component.hpp"

using std::vector;
using glm::vec2;

class Collider;

class Rigidbody : public Component {

    private:

        vector<Collider*> colliders;
        vec2 forceAccum;
        vec2 velocity;
        float cor;
        float mass;
        bool sensor;

    public:

        Rigidbody();
        ~Rigidbody();

        vector<Collider*> getColliders();
        vec2 getVelocity();
        float getCor();
        float getMass();
        bool isSensor();

        Rigidbody* addCollider(Collider* collider);
        Rigidbody* addColliders(vector<Collider*> colliders);
        Rigidbody* removeCollider(Collider* collider);
        Rigidbody* removeColliders(vector<Collider*> colliders);
        Rigidbody* setCollider(Collider* collider);
        Rigidbody* setColliders(vector<Collider*> colliders);
        Rigidbody* clearColliders();

        Rigidbody* setVelocity(vec2 velocity);
        Rigidbody* setCor(float cor);
        Rigidbody* setMass(float mass);
        Rigidbody* setSensor(bool sensor);

        void clearAccumulators();
        void physicsUpdate(float dt);
        void addVelocity(vec2 velocity);
        void addForce(vec2 force);
        void zeroForces();
        bool hasInfiniteMass();

};

#include "physics/primitives/collider.hpp"