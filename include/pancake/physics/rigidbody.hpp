#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_set>

#include <glm/glm.hpp>
#include "pancake/core/component.hpp"

namespace Pancake {

    class Collider;

    class Rigidbody : public Component {

        private:

            std::vector<Collider*> colliders;                // Gives a rigidbody shape.
            std::unordered_set<std::string> forceGenerators; // Allows specific force generators to act on the rigidbody.

            glm::vec2 force;
            glm::vec2 velocity;
            float torque;
            float angularVelocity;

            float restitution;
            float friction;

            glm::vec2 lastPosition;
            float lastRotation;

            glm::vec2 centroid;
            std::pair<glm::vec2, glm::vec2> bounds;
            float mass;
            float moment;
            bool centroidDirty;
            bool boundsDirty;
            bool massDirty;
            bool momentDirty;
            
            bool sensor;
            bool infiniteMass;
            bool fixedOrientation;

        public:

            Rigidbody();
            void start() override;
            void end() override;
            json serialise() override;
            bool load(json j) override;

            std::vector<Collider*> getColliders();
            std::unordered_set<std::string> getForceGenerators();
            glm::vec2 getVelocity();
            glm::vec2 getCentroid();
            std::pair<glm::vec2, glm::vec2> getBounds();
            float getAngularVelocity();
            float getRestitution();
            float getFriction();
            float getMass();
            float getInverseMass();
            float getMomentOfInertia();
            float getInverseMomentOfInertia();
            bool isSensor();
            bool hasForceGenerator(std::string type);
            bool hasFixedOrientation();
            bool hasInfiniteMass();

            Rigidbody* addCollider(Collider* collider);
            Rigidbody* addColliders(std::vector<Collider*> colliders);
            Rigidbody* removeCollider(Collider* collider);
            Rigidbody* removeColliders(std::vector<Collider*> colliders);
            Rigidbody* setCollider(Collider* collider);
            Rigidbody* setColliders(std::vector<Collider*> colliders);
            Rigidbody* clearColliders();

            Rigidbody* addForceGenerator(std::string type);
            Rigidbody* addForceGenerators(std::vector<std::string> types);
            Rigidbody* removeForceGenerator(std::string type);
            Rigidbody* removeForceGenerators(std::vector<std::string> types);
            Rigidbody* setForceGenerator(std::string type);
            Rigidbody* setForceGenerators(std::vector<std::string> types);
            Rigidbody* clearForceGenerators();

            Rigidbody* setForce(glm::vec2 force);
            Rigidbody* setForce(float x, float y);
            Rigidbody* setVelocity(glm::vec2 velocity);
            Rigidbody* setVelocity(float x, float y);
            Rigidbody* setTorque(float torque);
            Rigidbody* setAngularVelocity(float angularVelocity);
            Rigidbody* setRestitution(float cor);
            Rigidbody* setFriction(float cof);
            Rigidbody* setSensor(bool sensor);
            Rigidbody* setFixedOrientation(bool orientation);
            Rigidbody* setCentroidDirty();
            Rigidbody* setBoundsDirty();
            Rigidbody* setMassDirty();
            Rigidbody* setMomentDirty();

            void physicsUpdate(float dt);

            void clearAccumulators();
            void addVelocity(glm::vec2 velocity);
            void addVelocity(float x, float y);
            void addAngularVelocity(float angularVelocity);
            void addForce(glm::vec2 force);
            void addForce(float x, float y);
            void addTorque(float torque);
            void zeroForces();
            void zeroTorque();
            
    };

    REGISTER(Component, Rigidbody);

}

#include "pancake/physics/collider.hpp"