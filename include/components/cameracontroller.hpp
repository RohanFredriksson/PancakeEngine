#pragma once

#include <glm/glm.hpp>
#include "core/component.hpp"

using glm::vec2;

class CameraController : public Component {

    private:

        bool moving;
        vec2 initialPosition;
        vec2 finalPosition;
        vec2 normal;
        float timeCurrent;
        float timeTotal;
        float timeHalf;
        float distance;
        float maxVelocity;

    public:

        CameraController();
        void update(float dt) override;
        void moveTo(vec2 to, float t);

};