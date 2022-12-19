#pragma once

#include <glm/glm.hpp>
#include "core/component.hpp"

using glm::vec2;

class Collider;

class Rigidbody : public Component {

    private:

        Collider* collider;
        vec2 forceAccum;
        vec2 velocity;
        float cor;
        float mass;
        bool sensor;

    public:

        Rigidbody();
        ~Rigidbody();

        Collider* getCollider();
        vec2 getVelocity();
        float getCor();
        float getMass();
        bool isSensor();

        Rigidbody* setCollider(Collider* collider);
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