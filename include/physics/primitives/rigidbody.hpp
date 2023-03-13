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
        vec2 force;
        vec2 velocity;
        float torque;
        float angularVelocity;
        float restitution;
        bool sensor;
        bool fixedOrientation;

    public:

        Rigidbody();
        ~Rigidbody();

        vector<Collider*> getColliders();
        vec2 getVelocity();
        vec2 getCentroid();
        float getAngularVelocity();
        float getRestitution();
        float getMass();
        float getInverseMass();
        float getMomentOfInertia();
        float getInverseMomentOfInertia();
        bool isSensor();
        bool hasFixedOrientation();
        bool hasInfiniteMass();

        Rigidbody* addCollider(Collider* collider);
        Rigidbody* addColliders(vector<Collider*> colliders);
        Rigidbody* removeCollider(Collider* collider);
        Rigidbody* removeColliders(vector<Collider*> colliders);
        Rigidbody* setCollider(Collider* collider);
        Rigidbody* setColliders(vector<Collider*> colliders);
        Rigidbody* clearColliders();

        Rigidbody* setVelocity(vec2 velocity);
        Rigidbody* setAngularVelocity(float angularVelocity);
        Rigidbody* setRestitution(float cor);
        Rigidbody* setSensor(bool sensor);
        Rigidbody* setFixedOrientation(bool orientation);

        void physicsUpdate(float dt);

        void clearAccumulators();
        void addVelocity(vec2 velocity);
        void addAngularVelocity(float angularVelocity);
        void addForce(vec2 force);
        void addTorque(float torque);
        void zeroForces();
        void zeroTorque();
        

};

#include "physics/primitives/collider.hpp"