#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "pancake/core/component.hpp"

using std::vector;
using glm::vec2;

namespace Pancake {

    class Collider;

    class Rigidbody : public Component {

        private:

            vector<Collider*> colliders;

            vec2 force;
            vec2 velocity;
            float torque;
            float angularVelocity;

            float restitution;
            float friction;

            vec2 centroid;
            float mass;
            float moment;
            bool centroidDirty;
            bool massDirty;
            bool momentDirty;
            
            bool sensor;
            bool infiniteMass;
            bool fixedOrientation;

        public:

            Rigidbody();
            void end() override;
            json serialise() override;
            bool load(json j) override;

            vector<Collider*> getColliders();
            vec2 getVelocity();
            vec2 getCentroid();
            float getAngularVelocity();
            float getRestitution();
            float getFriction();
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

            Rigidbody* setForce(vec2 force);
            Rigidbody* setVelocity(vec2 velocity);
            Rigidbody* setTorque(float torque);
            Rigidbody* setAngularVelocity(float angularVelocity);
            Rigidbody* setRestitution(float cor);
            Rigidbody* setFriction(float cof);
            Rigidbody* setSensor(bool sensor);
            Rigidbody* setFixedOrientation(bool orientation);
            Rigidbody* setCentroidDirty();
            Rigidbody* setMassDirty();
            Rigidbody* setMomentDirty();

            void physicsUpdate(float dt);

            void clearAccumulators();
            void addVelocity(vec2 velocity);
            void addAngularVelocity(float angularVelocity);
            void addForce(vec2 force);
            void addTorque(float torque);
            void zeroForces();
            void zeroTorque();
            
    };

}

#include "pancake/physics/collider.hpp"