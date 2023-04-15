#include <vector>
#include <algorithm>
#include <cstdlib>
#include "pancake/graphics/shader.hpp"
#include "pancake/graphics/debugdraw.hpp"
#include "pancake/core/window.hpp"
#include "pancake/asset/shaders.hpp"

using std::vector;

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

    vec2 rotate(vec2 vector, float degrees, vec2 origin) {

        float x = vector.x - origin.x;
        float y = vector.y - origin.y;

        float cos = cosf(degrees * (M_PI / 180));
        float sin = sinf(degrees * (M_PI / 180));

        float xPrime = (x * cos) - (y * sin);
        float yPrime = (x * sin) + (y * cos);
        
        xPrime += origin.x;
        yPrime += origin.y;

        return glm::vec2(xPrime, yPrime);

    }

}

namespace DebugDraw {

    void init() {

        // Initialise required data.
        shader = new Shader("debug", "debug", DEBUG_VERTEX, DEBUG_FRAGMENT);
        vertices = (float*) malloc(VERTEX_ARRAY_LENGTH * sizeof(float));

        // Generate and bind a Vertex Array Object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Allocate space for vertices
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, MAX_LINES * VERTEX_ARRAY_LENGTH * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

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
        int n = lines.size();
        for (int i = 0; i < n; i++) {
            delete lines[i];
        }
    }

    void drawLine(vec2 from, vec2 to, vec3 colour, int lifetime) {

        if (lines.size() >= MAX_LINES) {return;}

        Camera* camera = Window::getScene()->getCamera();
        
        vec2 pMin = camera->getPosition() - (camera->getProjectionSize() * (0.5f / camera->getZoom()));
        vec2 pMax = camera->getPosition() + (camera->getProjectionSize() * (0.5f / camera->getZoom()));

        vec2 lMin = glm::vec2(std::min(from.x, to.x), std::min(from.y, to.y));
        vec2 lMax = glm::vec2(std::max(from.x, to.x), std::max(from.y, to.y));

        bool inView = lMax.x > pMin.x && pMax.x > lMin.x && lMax.y > pMin.y && pMax.y > lMin.y;
        if (!inView) {
            return;
        }

        Line* line = new Line(from, to, colour, lifetime);
        lines.push_back(line);

    }

    void drawBox(vec2 centre, vec2 dimensions, float rotation, vec3 colour, int lifetime) {

        vec2 min = centre - dimensions * 0.5f;
        vec2 max = centre + dimensions * 0.5f;

        vec2 vertices[4];
        vertices[0] = glm::vec2(min.x, min.y);
        vertices[1] = glm::vec2(min.x, max.y);
        vertices[2] = glm::vec2(max.x, max.y);
        vertices[3] = glm::vec2(max.x, min.y);

        if (rotation != 0.0f) {
            for (int i = 0; i < 4; i++) {
                vertices[i] = rotate(vertices[i], rotation * 180 / M_PI, centre);
            }
        }

        drawLine(vertices[0], vertices[1], colour, lifetime);
        drawLine(vertices[0], vertices[3], colour, lifetime);
        drawLine(vertices[1], vertices[2], colour, lifetime);
        drawLine(vertices[2], vertices[3], colour, lifetime);

    }

    void drawCircle(vec2 centre, float radius, vec3 colour, int lifetime) {

        vector<vec2> points;
        int increment = 360 / CIRCLE_POINTS;
        int currentAngle = 0;

        for (int i = 0; i < CIRCLE_POINTS; i++) {
            vec2 point = rotate(glm::vec2(0, radius), currentAngle, glm::vec2(0, 0)) + centre;
            points.push_back(point);
            if (i > 0) {drawLine(points[i - 1], points[i], colour, lifetime);}
            currentAngle += increment;
        }
        drawLine(points[CIRCLE_POINTS - 1], points[0], colour, lifetime);

    }

}

