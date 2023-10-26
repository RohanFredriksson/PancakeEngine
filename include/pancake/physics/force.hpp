#pragma once

#include "pancake/physics/rigidbody.hpp"
#include "pancake/core/factory.hpp"
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <string>

namespace Pancake {

    class ForceGenerator {

        private:

            std::string type;

            void init(std::string type);

        public:

            ForceGenerator(std::string type);
            virtual nlohmann::json serialise();
            virtual bool load(nlohmann::json j);
            virtual void updateForce(Rigidbody* rigidbody, float dt);

            std::string getType();

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

            glm::vec2 gravity;

        public:

            Gravity();
            nlohmann::json serialise() override;
            bool load(nlohmann::json j) override;
            void updateForce(Rigidbody* rigidbody, float dt) override;

            glm::vec2 getGravity();
            void setGravity(glm::vec2 gravity);

    };

    REGISTER(ForceGenerator, Gravity);

}