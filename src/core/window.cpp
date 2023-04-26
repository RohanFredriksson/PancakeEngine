#include <iostream>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "pancake/core/window.hpp"
#include "pancake/core/listener.hpp"
#include "pancake/graphics/shader.hpp"
#include "pancake/graphics/framebuffer.hpp"
#include "pancake/graphics/debugdraw.hpp"
#include "pancake/asset/assetpool.hpp"
#include "pancake/asset/shaders.hpp"

namespace Pancake {

    namespace {

        GLFWwindow* window;
        int width = 800;
        int height = 800;
        Scene* scene;

        bool saveFlag = false;
        string saveFilename;

        bool exitFlag = false;

        bool loadFlag;
        string loadName;
        string loadFilename;
        void(*loadInit)(Scene* scene) = nullptr;

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
            window = glfwCreateWindow(width, height, "Pancake", nullptr, nullptr);
            if (window == nullptr) {
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

            // Initialise ImGui
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            (void)io;
            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 330");

            // Enable alpha transparency
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

            // Imgui Render
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            Console::render();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

        void load(string name, string filename, void(*init)(Scene* scene)) {
            loadFlag = true;
            loadName = name;
            loadFilename = filename;
            loadInit = init;
        }

        void save(string filename) {
            saveFlag = true;
            saveFilename = filename;
        }

        void exit() {
            exitFlag = true;
        }

        void loop() {

            float beginTime = (float)glfwGetTime();
            float endTime = (float)glfwGetTime();
            float dt = -1.0f;

            while (!glfwWindowShouldClose(window) && !exitFlag) {

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

                if (saveFlag) {
                    scene->save(saveFilename);
                    saveFlag = false;
                }

                if (loadFlag) {
                    delete scene;
                    scene = new Scene(loadName, loadFilename, loadInit);
                    dt = -1.0f;
                    loadFlag = false;
                }

            }

            scene->save("saves/test.scene");
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

        void openConsole() {
            Console::open();
        }

        void closeConsole() {
            Console::close();
        }

    }

}