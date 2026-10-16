#include <iostream>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "pancake/core/window.hpp"
#include "pancake/core/inspector.hpp"
#include "pancake/core/listener.hpp"
#include "pancake/graphics/shader.hpp"
#include "pancake/graphics/framebuffer.hpp"
#include "pancake/graphics/debugdraw.hpp"
#include "pancake/asset/assetpool.hpp"
#include "pancake/asset/shaders.hpp"

namespace Pancake {

    namespace {

        GLFWwindow* window = nullptr;
        int width = 800;
        int height = 800;
        int framebufferWidth = 800;  // In pixels, which differs from the window size on high DPI displays.
        int framebufferHeight = 800;
        Scene* scene = nullptr;

        bool loadFlag = false;
        vector<string> loadFilenames;
        vector<std::function<void(Scene*)>> loadMethods;

        bool saveFlag = false;
        string saveFilename;

        bool resetFlag = false;

        bool stopFlag = false;

        bool titleFlag = false;
        string titleString;

        bool iconFlag = false;
        string iconFilename;

        bool projectionFlag = false;
        float projectionHeight;

        bool widthFlag = false;
        int widthValue;

        bool heightFlag = false;
        int heightValue;

        float timeScale = 1.0f;
        bool paused = false;
        int stepFrames = 0;
        float fps = 0.0f;
        float unscaledDeltaTime = 0.0f;

        bool vsync = true;
        bool vsyncFlag = false;

        bool fullscreen = false;
        bool fullscreenFlag = false;
        int windowedX, windowedY, windowedWidth, windowedHeight; // Restored when leaving fullscreen.

        Shader* defaultShader;
        Shader* entityShader;
        Framebuffer* entityTexture;

        void update(float dt) {
            scene->update(dt);
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
            Inspector::render();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);

        }

        // The time the scene advances by this frame.
        float sceneDeltaTime(float dt) {
            if (!paused) {return dt * timeScale;}
            if (stepFrames <= 0) {return 0.0f;}
            stepFrames--;
            return scene->getPhysics()->getTimeStep(); // Exactly one physics step, whatever the frame time.
        }

        void applyFullscreen() {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            if (fullscreen && monitor != nullptr) {
                glfwGetWindowPos(window, &windowedX, &windowedY);
                glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            } else if (fullscreen) {
                std::cout << "ERROR::WINDOW::NO_MONITOR_FOR_FULLSCREEN\n";
                fullscreen = false;
            } else if (glfwGetWindowMonitor(window) != nullptr) {
                glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, 0);
            }
            glfwSwapInterval(vsync ? 1 : 0); // Some platforms reset the swap interval when the monitor changes.
        }

        void resizeCallback(GLFWwindow* window, int screenWidth, int screenHeight) {
            width = screenWidth;
            height = screenHeight;
            if (scene != nullptr) {scene->getCamera()->adjustProjection();}
        }

        void framebufferResizeCallback(GLFWwindow* window, int pixelWidth, int pixelHeight) {
            if (pixelWidth <= 0 || pixelHeight <= 0) {return;} // Minimised.
            framebufferWidth = pixelWidth;
            framebufferHeight = pixelHeight;
            delete entityTexture;
            entityTexture = new Framebuffer(GL_RGB32F, framebufferWidth, framebufferHeight, GL_RGB, GL_FLOAT);
            glViewport(0, 0, framebufferWidth, framebufferHeight);
        }

    }

    namespace Window {

        void load(std::function<void(Scene*)> method) {
            loadFlag = true;
            loadMethods.push_back(method);
        }

        void load(string filename) {
            loadFlag = true;
            loadFilenames.push_back(filename);
        }

        void save(string filename) {
            saveFlag = true;
            saveFilename = filename;
        }

        void reset() {
            resetFlag = true;
        }

        void start() {

            // Allow the engine to be started again after a previous stop.
            stopFlag = false;

            // Initialise GLFW
            if (!glfwInit()) {
                std::cout << "ERROR::WINDOW::GLFW_INITIALIZATION_FAILED\n";
                return;
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
                return;
            }

            // Manage callbacks
            glfwSetKeyCallback(window, KeyListener::keyCallback);
            glfwSetWindowSizeCallback(window, resizeCallback);
            glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
            glfwSetCursorPosCallback(window, MouseListener::mousePosCallback);
            glfwSetMouseButtonCallback(window, MouseListener::mouseButtonCallback);
            glfwSetScrollCallback(window, MouseListener::mouseScrollCallback);

            // Make the OpenGl context current
            glfwMakeContextCurrent(window);

            // Enable v-sync, unless it was turned off before starting.
            glfwSwapInterval(vsync ? 1 : 0);
            vsyncFlag = false;

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

            // Start the scene
            AssetPool::init();
            defaultShader = new Shader("default", "default", DEFAULT_VERTEX, DEFAULT_FRAGMENT);
            entityShader = new Shader("default", "entity", DEFAULT_VERTEX, ENTITY_FRAGMENT);
            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
            glViewport(0, 0, framebufferWidth, framebufferHeight);
            entityTexture = new Framebuffer(GL_RGB32F, framebufferWidth, framebufferHeight, GL_RGB, GL_FLOAT);
            DebugDraw::init();
            scene = new Scene();
            scene->start();
            if (fullscreen) {fullscreenFlag = true;}
            
            // Loop
            float beginTime = (float)glfwGetTime();
            float endTime = (float)glfwGetTime();
            float dt = -1.0f;

            while (!glfwWindowShouldClose(window) && !stopFlag) {

                if (titleFlag) {
                    glfwSetWindowTitle(window, titleString.c_str());
                    titleFlag = false;
                }

                if (iconFlag) {

                    int width;
                    int height;
                    int channels;

                    stbi_set_flip_vertically_on_load(0);
                    unsigned char* image = stbi_load(iconFilename.c_str(), &width, &height, &channels, 0);

                    if (image != nullptr) {
                        GLFWimage icon;
                        icon.width = width;
                        icon.height = height;
                        icon.pixels = image;
                        glfwSetWindowIcon(window, 1, &icon);
                        stbi_image_free(image);
                    }

                    iconFlag = false;

                }

                // Apply both dimensions at once, as the resize callback that updates them arrives later.
                if (widthFlag || heightFlag) {
                    glfwSetWindowSize(window, widthFlag ? widthValue : width, heightFlag ? heightValue : height);
                    widthFlag = false;
                    heightFlag = false;
                }

                if (vsyncFlag) {
                    glfwSwapInterval(vsync ? 1 : 0);
                    vsyncFlag = false;
                }

                if (fullscreenFlag) {
                    applyFullscreen();
                    fullscreenFlag = false;
                }

                glfwPollEvents();
                GamepadListener::poll();

                if (saveFlag) {
                    scene->save(saveFilename);
                    saveFlag = false;
                }

                if (resetFlag) {
                    delete scene;
                    scene = new Scene();
                    scene->start();
                    dt = -1.0f;
                    resetFlag = false;
                }

                if (loadFlag) {
                    // Move the queues out before running them, so a load method that queues another load
                    // can't reallocate the vector holding the function that is running. Queued loads still run this frame.
                    while (!loadMethods.empty() || !loadFilenames.empty()) {
                        vector<std::function<void(Scene*)>> methods;
                        vector<string> filenames;
                        methods.swap(loadMethods);
                        filenames.swap(loadFilenames);
                        for (auto& method : methods) {method(scene);}
                        for (string& filename : filenames) {scene->load(filename);}
                    }
                    loadFlag = false;
                }

                if (projectionFlag) {
                    scene->getCamera()->setProjectionHeight(projectionHeight);
                    projectionFlag = false;
                }

                if (dt > 0) {
                    unscaledDeltaTime = dt;
                    fps = fps <= 0.0f ? 1.0f / dt : 0.95f * fps + 0.05f / dt;
                    update(sceneDeltaTime(dt));
                    render();
                }

                KeyListener::endFrame();
                MouseListener::endFrame();

                endTime = (float)glfwGetTime();
                dt = endTime - beginTime;
                beginTime = endTime;

            }

            // Destroy the scene first, as its components release resources held by other systems.
            delete scene;
            scene = nullptr;

            // Destroy
            delete entityTexture;
            delete entityShader;
            delete defaultShader;
            entityTexture = nullptr;
            entityShader = nullptr;
            defaultShader = nullptr;
            DebugDraw::destroy();
            AssetPool::destroy();

            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glfwDestroyWindow(window);
            window = nullptr;
            glfwTerminate();

        }

        void stop() {
            stopFlag = true;
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

        void setTitle(string title) {
            titleString = title;
            titleFlag = true;
        }

        void setIcon(string filename) {
            iconFilename = filename;
            iconFlag = true;
        }

        void setWidth(int w) {
            if (window == nullptr) {width = w; return;} // Not created yet, so create it at this size.
            widthValue = w;
            widthFlag = true;
        }

        void setHeight(int h) {
            if (window == nullptr) {height = h; return;}
            heightValue = h;
            heightFlag = true;
        }

        void setProjectionSize(vec2 size) {
            projectionHeight = size.y;
            projectionFlag = true;
        }

        void setProjectionSize(float height) {
            projectionHeight = height;
            projectionFlag = true;
        }

        void setProjectionHeight(float height) {
            projectionHeight = height;
            projectionFlag = true;
        }

        int readPixel(int x, int y) {

            // Create a buffer to store pixel data.
            float pixel;

            // Convert from window coordinates to framebuffer pixels.
            int px = (int) (x * (float) framebufferWidth / width);
            int py = (int) (y * (float) framebufferHeight / height);
            if (px < 0 || py < 0 || px >= framebufferWidth || py >= framebufferHeight) {return -1;}

            // Bind the framebuffer and read the corresponding pixel.
            entityTexture->bind();
            glReadPixels(px, framebufferHeight - py - 1, 1, 1, GL_RED, GL_FLOAT, &pixel);
            entityTexture->unbind();

            // Return the value of this pixel.
            return (int)pixel - 1;

        }

        void setTimeScale(float scale) {
            timeScale = scale < 0.0f ? 0.0f : scale;
        }

        float getTimeScale() {
            return timeScale;
        }

        void setPaused(bool state) {
            paused = state;
            if (!paused) {stepFrames = 0;}
        }

        bool isPaused() {
            return paused;
        }

        void step(int frames) {
            paused = true;
            if (frames > 0) {stepFrames += frames;}
        }

        float getFps() {
            return fps;
        }

        float getUnscaledDeltaTime() {
            return unscaledDeltaTime;
        }

        void setVsync(bool state) {
            vsync = state;
            vsyncFlag = true;
        }

        bool hasVsync() {
            return vsync;
        }

        void setFullscreen(bool state) {
            if (state == fullscreen) {return;}
            fullscreen = state;
            if (window != nullptr) {fullscreenFlag = true;} // Otherwise applied when the window is created.
        }

        bool isFullscreen() {
            return fullscreen;
        }

        void openConsole() {
            Console::open();
        }

        void closeConsole() {
            Console::close();
        }

    }

}