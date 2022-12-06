#include <vector>
#include <algorithm>
#include <stdlib.h>
#include "graphics/primitives/shader.hpp"
#include "graphics/renderer/debugdraw.hpp"
#include "core/window.hpp"

#include <stdio.h>

namespace {

    const int MAX_LINES = 5000;
    const int VERTEX_ARRAY_LENGTH = MAX_LINES * 6 * 2;
    const int CIRCLE_POINTS = 20;

    class Line {

        public:

            vec2 from;
            vec2 to;
            vec3 colour;
            int lifetime;

            Line(vec2 from, vec2 to, vec3 colour, int lifetime) {
                this->from = from;
                this->to = to;
                this->colour = colour;
                this->lifetime = lifetime;
            }

    };

    vector<Line*> lines;
    float* vertices;
    Shader* shader;
    unsigned int vao;
    unsigned int vbo;

}

namespace DebugDraw {

    void init() {

        // Initialise required data.
        shader = new Shader("assets/shaders/debug.vert", "assets/shaders/debug.frag");
        vertices = (float*) malloc(VERTEX_ARRAY_LENGTH * sizeof(float));

        // Generate and bind a Vertex Array Object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Allocate space for vertices
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, MAX_LINES * VERTEX_ARRAY_LENGTH * sizeof(float), NULL, GL_DYNAMIC_DRAW);

        // Enable the vertex array attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glLineWidth(2.0f);

    }

    void render() {

        for (int i = 0; i < lines.size(); i++) {
            Line* current = lines[i];
            current->lifetime--;
            if (current->lifetime < 0) {
                delete current;
                lines.erase(lines.begin() + i);
                i--;
            }
        }

        if (lines.size() == 0) {return;}

        int index = 0;
        int n = lines.size();
        for (int i = 0; i < n; i++) {

            Line* current = lines[i];
            for (int j = 0; j < 2; j++) {

                vec2 position;
                if (j == 0) {position = current->from;}
                else {position = current->to;}

                vec3 colour = current->colour;

                // Load positions.
                vertices[index + 0] = position.x;
                vertices[index + 1] = position.y;
                vertices[index + 2] = -10.0f;

                // Load colour
                vertices[index + 3] = colour.x;
                vertices[index + 4] = colour.y;
                vertices[index + 5] = colour.z;
                index = index + 6;

            }

        }      

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, VERTEX_ARRAY_LENGTH * sizeof(float), vertices, GL_DYNAMIC_DRAW);

        shader->bind();
        shader->uploadMat4("uProjection", Window::getScene()->getCamera()->getProjection());
        shader->uploadMat4("uView", Window::getScene()->getCamera()->getView());

        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_LINES, 0, 2 * lines.size());

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindVertexArray(0);

        shader->unbind();

    }

    void destroy() {
        delete shader;
        free(vertices);
    }

    void drawLine(vec2 from, vec2 to, vec3 colour, int lifetime) {

        if (lines.size() >= MAX_LINES) {return;}

        /*
        Camera* camera = Window::getScene()->getCamera();
        
        vec2 pMin = (camera->getProjectionSize() * (0.5f / camera->getZoom())) - camera->getPosition();
        vec2 pMax = (camera->getProjectionSize() * (0.5f / camera->getZoom())) + camera->getPosition();

        vec2 lMin = glm::vec2(std::min(from.x, to.x), std::min(from.y, to.y));
        vec2 lMax = glm::vec2(std::max(from.x, to.x), std::max(from.y, to.y));

        bool inView = lMax.x > pMin.x && pMax.x > lMin.x && lMax.y > pMin.y && pMax.y > lMin.y;
        if (!inView) {
            return;
        }
        */

        Line* line = new Line(from, to, colour, lifetime);
        lines.push_back(line);

    }

    void drawBox(vec2 centre, vec2 dimensions, float rotation, vec3 colour, int lifetime) {

    }

    void drawCircle(vec2 centre, float radius, vec3 colour, int lifetime) {

    }

}

