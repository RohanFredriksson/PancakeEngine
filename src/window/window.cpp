#include <iostream>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "pancake/window/window.hpp"
#include "pancake/window/listener.hpp"
#include "pancake/graphics/primitives/shader.hpp"
#include "pancake/graphics/primitives/framebuffer.hpp"
#include "pancake/graphics/renderer/debugdraw.hpp"
#include "pancake/util/assetpool.hpp"
#include "pancake/util/shaders.hpp"

namespace {

    GLFWwindow* window;
    int width = 800;
    int height = 800;
    Scene* scene;
    Shader* defaultShader;
    Shader* entityShader;
    Framebuffer* entityTexture;

    bool start() {

        // Initialise GLFW
        if (!glfwInit()) {
            std::cout << "ERROR::WINDOW::GLFW_INITIALIZATION_FAILED\n";
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
        window = glfwCreateWindow(width, height, "Pancake", NULL, NULL);
        if (window == NULL) {
            std::cout << "ERROR::WINDOW::GLFW_WINDOW_CREATION_FAILED\n";
            return false;
        }

        // Manage callbacks
        glfwSetKeyCallback(window, KeyListener::keyCallback);
        glfwSetWindowSizeCallback(window, WindowListener::resizeCallback);
        glfwSetCursorPosCallback(window, MouseListener::mousePosCallback);
        glfwSetMouseButtonCallback(window, MouseListener::mouseButtonCallback);
        glfwSetScrollCallback(window, MouseListener::mouseScrollCallback);

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

        // Initialise the asset pool.
        AssetPool::init();

        defaultShader = new Shader("default", "default", DEFAULT_VERTEX, DEFAULT_FRAGMENT);
        entityShader = new Shader("default", "entity", DEFAULT_VERTEX, ENTITY_FRAGMENT);
        entityTexture = new Framebuffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);

        DebugDraw::init();
        return true;

    }

    void update(float dt) {
        
        scene->update(dt);

        if (MouseListener::isMouseDragging()) {
            if (MouseListener::getDx() != 0) {scene->getCamera()->addPosition(vec2(-MouseListener::getWorldDx(), 0.0f));}
            if (MouseListener::getDy() != 0) {scene->getCamera()->addPosition(vec2(0.0f, MouseListener::getWorldDy()));}
        }

        //if (MouseListener::isMouseButtonBeginDown(GLFW_MOUSE_BUTTON_LEFT)) {
        //    std::cout << Window::readPixel(MouseListener::getX(), MouseListener::getY());
        //}

    }

    void render() {

        // Render the scene to the entity picking texture.
        glDisable(GL_BLEND);
        entityTexture->bind();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Renderer::bindShader(entityShader);
        scene->render();
        entityTexture->unbind();
        glEnable(GL_BLEND);

        // Render the scene to the window.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Renderer::bindShader(defaultShader);
        scene->render();

        // Debug draw
        DebugDraw::render();

        glfwSwapBuffers(window);

    }

}

namespace Window {

    bool init(string name, string filename, void(*init)(Scene* scene)) {
        if (!start()) {return false;}
        try {scene = new Scene(name, filename, init);}
        catch (...) {return false;}
        return true;
    }

    bool init(string name, void(*init)(Scene* scene)) {
        if (!start()) {return false;}
        try {scene = new Scene(name, init);}
        catch (...) {return false;}
        return true;
    }

    bool init(string name, string filename) {
        if (!start()) {return false;}
        try {scene = new Scene(name, filename);}
        catch (...) {return false;}
        return true;
    }

    void loop() {

        // Temporary audio test.
        //AudioEngine::play(AudioPool::get("assets/audio/ui.mp3"));

        float beginTime = (float)glfwGetTime();
        float endTime = (float)glfwGetTime();
        float dt = -1.0f;

        while (!glfwWindowShouldClose(window)) {

            glfwPollEvents();

            if (dt > 0) {
                update(dt);
                render();
            }

            MouseListener::endFrame();
            Window::readPixel(0, 0);

            endTime = (float)glfwGetTime();
            dt = endTime - beginTime;
            beginTime = endTime;

        }

        scene->save("saves/title.scene");

    }

    void destroy() {
        DebugDraw::destroy();
        AssetPool::destroy();
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    int getWidth() {
        return width;
    }

    int getHeight() {
        return height;
    }

    float getAspectRatio() {
        return (float) width / (float) height;
    }

    Scene* getScene() {
        return scene;
    }

    void setWidth(int w) {
        width = w;
    }

    void setHeight(int h) {
        height = h;
    }

    void resetFramebuffers() {
        delete entityTexture;
        entityTexture = new Framebuffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
    }

    int readPixel(int x, int y) {

        // Create a buffer to store pixel data.
        float pixel;

        // Bind the framebuffer and read the corresponding pixel.
        entityTexture->bind();
        glReadPixels(x, height-y-1, 1, 1, GL_RED, GL_FLOAT, &pixel);
        entityTexture->unbind();

        // Return the value of this pixel.
        return (int)pixel - 1;

    }

}