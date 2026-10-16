#include <cmath>
#include <glad/glad.h>
#include "pancake/core/listener.hpp"
#include "pancake/core/camera.hpp"
#include "pancake/core/window.hpp"

namespace {

    bool keyDown[350] = {0};
    bool keyBeginDown[350] = {0};
    bool keyEndDown[350] = {0};
    int keyPressCount = 0;

    double scrollX = 0.0;
    double scrollY = 0.0;
    double x = 0.0; 
    double y = 0.0; 
    double lastX = 0.0; 
    double lastY = 0.0; 
    double worldX = 0.0; 
    double worldY = 0.0; 
    double lastWorldX = 0.0; 
    double lastWorldY = 0.0;
    bool mouseDown[9] = {0};
    bool mouseBeginDown[9] = {0};
    bool mouseEndDown[9] = {0};
    int mouseDownCount = 0;
    bool mouseDragging = 0;

    struct Gamepad {
        bool connected = false;
        std::string name;
        GLFWgamepadstate state = {};
        GLFWgamepadstate previous = {};
    };

    Gamepad gamepads[GLFW_JOYSTICK_LAST + 1];
    float gamepadDeadZone = 0.15f;

    bool validGamepad(int gamepad) {
        return gamepad >= 0 && gamepad <= GLFW_JOYSTICK_LAST;
    }

    // Removes the dead zone and rescales, so values start at 0 just outside it and still reach 1.
    float removeDeadZone(float value) {
        float magnitude = std::fabs(value);
        if (magnitude <= gamepadDeadZone) {return 0.0f;}
        float scaled = std::fmin((magnitude - gamepadDeadZone) / (1.0f - gamepadDeadZone), 1.0f);
        return value < 0.0f ? -scaled : scaled;
    }

    // Applies the dead zone to the stick's length, so diagonals aren't snapped to the axes.
    glm::vec2 stick(int gamepad, int xAxis, int yAxis) {
        if (!validGamepad(gamepad) || !gamepads[gamepad].connected) {return glm::vec2(0.0f, 0.0f);}
        glm::vec2 v = glm::vec2(gamepads[gamepad].state.axes[xAxis], -gamepads[gamepad].state.axes[yAxis]);
        float length = glm::length(v);
        if (length <= gamepadDeadZone) {return glm::vec2(0.0f, 0.0f);}
        float scaled = std::fmin((length - gamepadDeadZone) / (1.0f - gamepadDeadZone), 1.0f);
        return v * (scaled / length);
    }
    
}

namespace Pancake {

    void KeyListener::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

        if (key < 0 || key > 349) {return;}

        if (action == GLFW_PRESS) {
            if (!keyDown[key]) {keyPressCount++;}
            keyDown[key] = true;
            keyBeginDown[key] = true;
        }

        else if (action == GLFW_RELEASE) {
            if (keyDown[key]) {keyPressCount--;}
            keyDown[key] = false;
            keyEndDown[key] = true;
        }

    }

    bool KeyListener::isKeyDown(int key) {
        if (key < 0 || key > 349) {return false;}
        return keyDown[key];
    }

    bool KeyListener::isKeyDown() {
        return keyPressCount > 0;
    }

    bool KeyListener::isKeyBeginDown(int key) {
        if (key < 0 || key > 349) {return false;}
        return keyBeginDown[key];
    }

    bool KeyListener::isKeyEndDown(int key) {
        if (key < 0 || key > 349) {return false;}
        return keyEndDown[key];
    }

    void KeyListener::endFrame() {
        // A key is only "beginning down" or "ending down" for the frame it was pressed or released in.
        // Both are kept until here, so a tap that presses and releases within one frame is still seen.
        for (int i = 0; i < 350; i++) {
            keyBeginDown[i] = false;
            keyEndDown[i] = false;
        }
    }

    void MouseListener::calcOrthoX() {

        if (Window::getScene() == nullptr || Window::getWidth() <= 0) {return;}
        float currentX = ((float) x / Window::getWidth()) * 2.0f - 1.0f;
        Camera* camera = Window::getScene()->getCamera();
        worldX = camera->getPosition().x + (currentX * 0.5f) * (camera->getProjectionSize().x / camera->getZoom());

    }

    void MouseListener::calcOrthoY() {

        if (Window::getScene() == nullptr || Window::getHeight() <= 0) {return;}
        float currentY = ((float) y / Window::getHeight()) * 2.0f - 1.0f;
        Camera* camera = Window::getScene()->getCamera();
        worldY = camera->getPosition().y + (currentY * -0.5f) * (camera->getProjectionSize().y / camera->getZoom());

    }

    void MouseListener::mousePosCallback(GLFWwindow* window, double xPos, double yPos) {

        if (mouseDownCount > 0) {
            mouseDragging = true;
        }

        lastX = x;
        lastY = y;
        lastWorldX = worldX;
        lastWorldY = worldY;
        x = xPos;
        y = yPos;
        calcOrthoX();
        calcOrthoY();

    }

    void MouseListener::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

        if (button > 8 || button < 0) {return;}

        if (action == GLFW_PRESS) {
            if (!mouseDown[button]) {mouseDownCount++;}
            mouseDown[button] = true;
            mouseBeginDown[button] = true;
        }

        else if (action == GLFW_RELEASE) {
            if (mouseDown[button]) {mouseDownCount--;}
            mouseDown[button] = false;
            mouseEndDown[button] = true;
            mouseDragging = false;
        }

    }

    void MouseListener::mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
        scrollX = xOffset;
        scrollY = yOffset;
    }

    bool MouseListener::isMouseDown(int button) {
        if (button < 0 || button > 8) {return false;}
        return mouseDown[button];
    }

    bool MouseListener::isMouseDown() {
        return mouseDownCount > 0;
    }

    bool MouseListener::isMouseBeginDown(int button) {

        if (button < 0 || button > 8) {return false;}
        return mouseBeginDown[button];
    }

    bool MouseListener::isMouseEndDown(int button) {
        if (button < 0 || button > 8) {return false;}
        return mouseEndDown[button];
    }

    bool MouseListener::hasMouseMoved() {
        return x != lastX || y != lastY;
    }

    bool MouseListener::isMouseDragging() {
        return mouseDragging;
    }

    double MouseListener::getX() {
        return x;
    }

    double MouseListener::getY() {
        return y;
    }

    double MouseListener::getDx() {
        return x - lastX;
    }

    double MouseListener::getDy() {
        return y - lastY;
    }

    double MouseListener::getWorldX() {
        return worldX;
    }

    double MouseListener::getWorldY() {
        return worldY;
    }

    double MouseListener::getWorldDx() {
        return worldX - lastWorldX;
    }

    double MouseListener::getWorldDy() {
        return worldY - lastWorldY;
    }

    double MouseListener::getScrollX() {
        return scrollX;
    }

    double MouseListener::getScrollY() {
        return scrollY;
    }

    void GamepadListener::poll() {
        for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++) {
            GLFWgamepadstate state = {};
            bool connected = glfwJoystickIsGamepad(i) && glfwGetGamepadState(i, &state);
            const char* name = connected ? glfwGetGamepadName(i) : nullptr;
            setState(i, connected, state, name == nullptr ? "" : name);
        }
    }

    void GamepadListener::setState(int gamepad, bool connected, const GLFWgamepadstate& state, std::string name) {
        if (!validGamepad(gamepad)) {return;}
        Gamepad& g = gamepads[gamepad];
        g.previous = g.state;
        g.state = connected ? state : GLFWgamepadstate {}; // A disconnected pad releases every button.
        g.connected = connected;
        g.name = connected ? name : "";
    }

    bool GamepadListener::isConnected(int gamepad) {
        return validGamepad(gamepad) && gamepads[gamepad].connected;
    }

    std::string GamepadListener::getName(int gamepad) {
        return validGamepad(gamepad) ? gamepads[gamepad].name : "";
    }

    bool GamepadListener::isButtonDown(int button, int gamepad) {
        if (!validGamepad(gamepad) || button < 0 || button > GLFW_GAMEPAD_BUTTON_LAST) {return false;}
        return gamepads[gamepad].state.buttons[button] == GLFW_PRESS;
    }

    bool GamepadListener::isButtonBeginDown(int button, int gamepad) {
        if (!validGamepad(gamepad) || button < 0 || button > GLFW_GAMEPAD_BUTTON_LAST) {return false;}
        return gamepads[gamepad].state.buttons[button] == GLFW_PRESS && gamepads[gamepad].previous.buttons[button] != GLFW_PRESS;
    }

    bool GamepadListener::isButtonEndDown(int button, int gamepad) {
        if (!validGamepad(gamepad) || button < 0 || button > GLFW_GAMEPAD_BUTTON_LAST) {return false;}
        return gamepads[gamepad].state.buttons[button] != GLFW_PRESS && gamepads[gamepad].previous.buttons[button] == GLFW_PRESS;
    }

    float GamepadListener::getAxis(int axis, int gamepad) {
        if (!validGamepad(gamepad) || !gamepads[gamepad].connected || axis < 0 || axis > GLFW_GAMEPAD_AXIS_LAST) {return 0.0f;}
        float value = gamepads[gamepad].state.axes[axis];
        if (axis == GLFW_GAMEPAD_AXIS_LEFT_TRIGGER || axis == GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER) {return 0.5f * (value + 1.0f);}
        return removeDeadZone(value);
    }

    glm::vec2 GamepadListener::getLeftStick(int gamepad) {
        return stick(gamepad, GLFW_GAMEPAD_AXIS_LEFT_X, GLFW_GAMEPAD_AXIS_LEFT_Y);
    }

    glm::vec2 GamepadListener::getRightStick(int gamepad) {
        return stick(gamepad, GLFW_GAMEPAD_AXIS_RIGHT_X, GLFW_GAMEPAD_AXIS_RIGHT_Y);
    }

    void GamepadListener::setDeadZone(float deadZone) {
        gamepadDeadZone = std::fmin(std::fmax(deadZone, 0.0f), 0.95f);
    }

    float GamepadListener::getDeadZone() {
        return gamepadDeadZone;
    }

    void MouseListener::endFrame() {

        for (int i = 0; i < 9; i++) {
            mouseBeginDown[i] = false;
            mouseEndDown[i] = false;
        }
        scrollX = 0.0;
        scrollY = 0.0;
        lastX = x;
        lastY = y;
        lastWorldX = worldX;
        lastWorldY = worldY;
        calcOrthoX();
        calcOrthoY();

    }

}