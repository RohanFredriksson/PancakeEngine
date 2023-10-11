#pragma once

#include <string>
#include <glm/glm.hpp>

#include "pancake/core/component.hpp"
#include "pancake/core/factory.hpp"

using glm::vec2;
using std::string;

namespace Pancake {

    class Rigidbody;

    class Collider {

        private:

            Rigidbody* rigidbody;
            string type;
            float mass;
            vec2 positionOffset;
            float rotationOffset;

            void init(string type, float mass, vec2 positionOffset, float rotationOffset);

        public:

            Collider(string type);
            virtual ~Collider();
            virtual json serialise();
            virtual bool load(json j);

            string getType();
            Rigidbody* getRigidbody();
            float getMass();
            virtual float getMomentOfInertia();
            vec2 getPosition();
            vec2 getPositionOffset();
            float getRotation();
            float getRotationOffset();
            bool hasInfiniteMass();

            Collider* setRigidbody(Rigidbody* rigidbody);
            Collider* setMass(float mass);
            Collider* setPositionOffset(float x, float y);
            Collider* setPositionOffset(vec2 offset);
            Collider* setRotationOffset(float offset);
            Collider* setPositionOffset(vec2 offset, bool update);
            Collider* setRotationOffset(float offset, bool update);

    };

    class Box : public Collider {

        private:

            vec2 size;
            vec2 halfSize;

        public:

            Box();
            json serialise() override;
            bool load(json j) override;

            float getMomentOfInertia() override;
            vec2 getSize();
            vec2 getHalfSize();
            vec2 getLocalMin();
            vec2 getLocalMax();

            Box* setSize(vec2 size);
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
            float getRadius();
            Circle* setRadius(float radius);
    };

    REGISTER(Collider, Circle);

    namespace ColliderFactory {
        void add(string type, void* (*create)());
        Collider* create(string type);
        Collider* load(json);
    }

}

#include "pancake/physics/rigidbody.hpp"