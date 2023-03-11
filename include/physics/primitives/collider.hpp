#pragma once

#include <glm/glm.hpp>
#include "core/component.hpp"

using glm::vec2;

class Rigidbody;

class Collider {

    private:

        Rigidbody* rigidbody;
        vec2 positionOffset;
        float rotationOffset;

    public:

        Collider();
        virtual ~Collider();

        Rigidbody* getRigidbody();
        Entity* getEntity();
        vec2 getPosition();
        vec2 getPositionOffset();
        float getRotation();
        float getRotationOffset();

        Collider* setRigidbody(Rigidbody* rigidbody);
        Collider* setPositionOffset(vec2 offset);
        Collider* setRotationOffset(float offset);

};

#include "physics/primitives/rigidbody.hpp"