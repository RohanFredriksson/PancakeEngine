#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Pancake {

    namespace KeyListener {
        void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        bool isKeyDown(int key);
        bool isKeyDown();
        bool isKeyBeginDown(int key);
        bool isKeyEndDown(int key);
        void endFrame();
    }

    namespace MouseListener {
        void calcOrthoX();
        void calcOrthoY();
        void mousePosCallback(GLFWwindow* window, double xPos, double yPos);
        void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
        bool isMouseDown(int button);
        bool isMouseDown();
        bool isMouseBeginDown(int button);
        bool isMouseEndDown(int button);
        bool hasMouseMoved();
        bool isMouseDragging();
        double getX();
        double getY();
        double getDx();
        double getDy();
        double getWorldX();
        double getWorldY();
        double getWorldDx();
        double getWorldDy();
        double getScrollX();
        double getScrollY();
        void endFrame();
    }

    // Gamepads with a standard mapping, polled once a frame. Buttons and axes use GLFW's names, such as
    // GLFW_GAMEPAD_BUTTON_A and GLFW_GAMEPAD_AXIS_LEFT_X. Gamepads are numbered 0 to GLFW_JOYSTICK_LAST.
    namespace GamepadListener {

        void poll();

        // Replace a gamepad's state, as poll does. Begin and end down compare it with the previous state.
        void setState(int gamepad, bool connected, const GLFWgamepadstate& state, std::string name = "");

        bool isConnected(int gamepad = 0);
        std::string getName(int gamepad = 0);
        bool isButtonDown(int button, int gamepad = 0);
        bool isButtonBeginDown(int button, int gamepad = 0);
        bool isButtonEndDown(int button, int gamepad = 0);

        // Stick axes are in [-1, 1] with the dead zone removed. Triggers are in [0, 1], at rest at 0.
        float getAxis(int axis, int gamepad = 0);
        glm::vec2 getLeftStick(int gamepad = 0);  // Up is positive y, unlike GLFW's raw axes.
        glm::vec2 getRightStick(int gamepad = 0);

        void setDeadZone(float deadZone);
        float getDeadZone();

    }

}