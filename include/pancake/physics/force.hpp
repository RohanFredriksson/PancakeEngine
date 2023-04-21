#pragma once

#include "pancake/physics/rigidbody.hpp"
#include <glm/glm.hpp>
#include <vector>

using std::vector;
using glm::vec2;

namespace Pancake {

    class ForceGenerator {

        public:
            virtual void updateForce(Rigidbody* rigidbody, float dt) = 0;

    };

    class ForceRegistry;

    class ForceRegistration {

        public:

            ForceGenerator* generator;
            Rigidbody* rigidbody;

            ForceRegistration(ForceGenerator* generator, Rigidbody* rigidbody);

    };

    class ForceRegistry {

        private:
            vector<ForceRegistration> registry;

        public:

            void add(ForceGenerator* generator, Rigidbody* rigidbody);
            void remove(ForceGenerator* generator, Rigidbody* rigidbody);
            void clear();
            void updateForces(float dt);
            void zeroForces();

    };

    class Gravity : public ForceGenerator {

        private:

            vec2 gravity;

        public:

            Gravity(vec2 gravity);
            void updateForce(Rigidbody* rigidbody, float dt) override;

            vec2 getGravity();
            void setGravity(vec2 gravity);

    };

}