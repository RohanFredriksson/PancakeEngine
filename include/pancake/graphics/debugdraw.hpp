#pragma once

#include <glm/glm.hpp>

using glm::vec2;
using glm::vec3;

namespace Pancake {

    namespace DebugDraw {

        void init();
        void render();
        void destroy();

        void drawLine(vec2 from, vec2 to, vec3 colour, int lifetime);
        void drawLine(vec2 from, vec2 to, vec3 colour);
        void drawLine(vec2 from, vec2 to);

        void drawBox(vec2 centre, vec2 dimensions, float rotation, vec3 colour, int lifetime);
        void drawBox(vec2 centre, vec2 dimensions, float rotation, vec3 colour);
        void drawBox(vec2 centre, vec2 dimensions, float rotation);
        void drawBox(vec2 centre, vec2 dimensions, vec3 colour);
        void drawBox(vec2 centre, vec2 dimensions);

        void drawCircle(vec2 centre, float radius, vec3 colour, int lifetime);
        void drawCircle(vec2 centre, float radius, vec3 colour);
        void drawCircle(vec2 centre, float radius);

    }

}