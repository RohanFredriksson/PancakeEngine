#pragma once

#include "pancake/physics/rigidbody.hpp"
#include "pancake/core/component.hpp"
#include "pancake/core/factory.hpp"
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <string>

namespace Pancake {

    class ForceGenerator : public Component {

        public:

            ForceGenerator(std::string type);
            void start() override;
            void end() override;

            virtual void updateForce(Rigidbody* rigidbody, float dt);

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
            std::vector<ForceRegistration> registry;

        public:

            void add(ForceGenerator* generator, Rigidbody* rigidbody);
            void remove(ForceGenerator* generator, Rigidbody* rigidbody);
            void remove(ForceGenerator* generator);
            void clear();
            void updateForces(float dt);
            void zeroForces();

    };

    class Gravity : public ForceGenerator {

        private:

            glm::vec2 acceleration;

        public:

            Gravity();
            nlohmann::json serialise() override;
            bool load(nlohmann::json j) override;
            void updateForce(Rigidbody* rigidbody, float dt) override;

            glm::vec2 getAcceleration();
            void setAcceleration(glm::vec2 acceleration);
            void setAcceleration(float x, float y);

    };

    REGISTER(Component, Gravity);

}