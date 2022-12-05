#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "core/window.hpp"
#include "graphics/primitives/shader.hpp"

namespace {

    GLFWwindow* window;
    Scene* scene;
    Shader* shader;
    float aspectRatio;

}

namespace Window {

    bool init() {

        // Initialise GLFW
        if (!glfwInit()) {
            printf("ERROR::WINDOW::GLFW_INITIALIZATION_FAILED\n");
            return false;
        }

        // Configure GLFW
        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

        // Create the window
        window = glfwCreateWindow(800, 800, "Pancake", NULL, NULL);
        if (window == NULL) {
            printf("ERROR::WINDOW::GLFW_WINDOW_CREATION_FAILED\n");
            return false;
        }

        // Make the OpenGl context current
        glfwMakeContextCurrent(window);

        // Enable v-sync
        glfwSwapInterval(1);

        // Make the window visible
        glfwShowWindow(window);

        // Load GLAD so it configures OpenGL
        gladLoadGL();

        // Enable alpha transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        scene = new Scene("Title");
        shader = new Shader("assets/shaders/default.vert", "assets/shaders/default.frag");

        return true;
    }

    void loop() {

        float beginTime = (float)glfwGetTime();
        float endTime = (float)glfwGetTime();
        float dt = -1.0f;

        while (!glfwWindowShouldClose(window)) {

            glfwPollEvents();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            scene->update(dt);

            Renderer::bindShader(shader);
            scene->render();

            glfwSwapBuffers(window);

            endTime = (float)glfwGetTime();
            dt = endTime - beginTime;
            beginTime = endTime;

        }

    }

    void exit() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void run() {
        if (!init()) {return;}
        loop();
        exit();
    }

    Scene* getScene() {
        return scene;
    }

}