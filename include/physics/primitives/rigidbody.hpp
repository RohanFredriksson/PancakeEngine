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

        void setCollider(Collider* collider);
        void setVelocity(vec2 velocity);
        void setCor(float cor);
        void setMass(float mass);
        void setSensor(bool sensor);

        void clearAccumulators();
        void physicsUpdate(float dt);
        void addForce(vec2 force);
        void zeroForces();
        bool hasInfiniteMass();

};

#include "physics/primitives/collider.hpp"