#pragma once

#include <string>
#include <glm/glm.hpp>
#include "pancake/core/component.hpp"

using glm::vec2;
using std::string;

namespace Pancake {

    class Rigidbody;

    class Collider {

        protected:

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
            Collider* setPositionOffset(vec2 offset);
            Collider* setRotationOffset(float offset);
            Collider* setPositionOffset(vec2 offset, bool update);
            Collider* setRotationOffset(float offset, bool update);

    };

    namespace ColliderFactory {
        void add(string type, void* (*create)());
        Collider* create(string type);
        Collider* load(json);
    }

}

#include "pancake/physics/rigidbody.hpp"