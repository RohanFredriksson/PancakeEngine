#pragma once

#include <string>
#include <utility>
#include <glm/glm.hpp>

#include "pancake/core/component.hpp"
#include "pancake/core/factory.hpp"

namespace Pancake {

    class Rigidbody;

    class Collider {

        private:

            Rigidbody* rigidbody;
            std::string type;
            float mass;
            glm::vec2 positionOffset;
            float rotationOffset;

            void init(string type, float mass, vec2 positionOffset, float rotationOffset);

        public:

            Collider(string type);
            virtual ~Collider();
            virtual json serialise();
            virtual bool load(json j);

            std::string getType();
            Rigidbody* getRigidbody();
            float getMass();
            virtual float getMomentOfInertia();
            virtual std::pair<glm::vec2, glm::vec2> getLocalBounds();
            glm::vec2 getPosition();
            glm::vec2 getPositionOffset();
            float getRotation();
            float getRotationOffset();
            bool hasInfiniteMass();

            Collider* setRigidbody(Rigidbody* rigidbody);
            Collider* setMass(float mass);
            Collider* setPositionOffset(float x, float y);
            Collider* setPositionOffset(glm::vec2 offset);
            Collider* setRotationOffset(float offset);
            Collider* setPositionOffset(glm::vec2 offset, bool update);
            Collider* setRotationOffset(float offset, bool update);

    };

    class Box : public Collider {

        private:
            vec2 size;

        public:

            Box();
            json serialise() override;
            bool load(json j) override;

            float getMomentOfInertia() override;
            std::pair<glm::vec2, glm::vec2> getLocalBounds() override;
            glm::vec2 getSize();

            Box* setSize(glm::vec2 size);
            Box* setSize(float w, float h);

    };

    REGISTER(Collider, Box);

    class Circle : public Collider {

        private:

            float radius;

        public:

            Circle();
            json serialise() override;
            bool load(json j) override;

            float getMomentOfInertia() override;
            std::pair<glm::vec2, glm::vec2> getLocalBounds() override;
            float getRadius();

            Circle* setRadius(float radius);

    };

    REGISTER(Collider, Circle);

}

#include "pancake/physics/rigidbody.hpp"