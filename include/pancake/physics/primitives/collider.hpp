#pragma once

#include <glm/glm.hpp>
#include "pancake/core/component.hpp"

using glm::vec2;

class Rigidbody;

class Collider {

    protected:

        Rigidbody* rigidbody;
        float mass;
        vec2 positionOffset;
        float rotationOffset;

    public:

        Collider();
        virtual ~Collider();
        virtual json serialise();

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

#include "pancake/physics/primitives/rigidbody.hpp"