#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

using glm::vec2;
using glm::mat4;
using json = nlohmann::json;

namespace Pancake {

    class Camera {

        private:

            vec2 position;
            vec2 projectionSize;
            float zoom;

            mat4 projection;
            mat4 view;
            mat4 inverseProjection;
            mat4 inverseView;

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

            Camera(vec2 position, vec2 projectionSize, float zoom);
            void adjustProjection();
            void update(float dt);
            void move(vec2 to, float t);
            json serialise();
            void load(json j);

            vec2 getPosition();
            vec2 getProjectionSize();
            mat4 getProjection();
            mat4 getView();
            mat4 getInverseProjection();
            mat4 getInverseView();
            float getZoom();

            void setPosition(vec2 position);
            void setProjectionSize(vec2 projectionSize);
            void setZoom(float zoom);

            void addPosition(vec2 position);
            void addZoom(float zoom);
            void scaleZoom(float scale);

    };

}