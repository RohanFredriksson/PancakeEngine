#include <glad/glad.h>
#include "pancake/core/listener.hpp"
#include "pancake/core/camera.hpp"
#include "pancake/core/window.hpp"

namespace {

    bool keyDown[350] = {0};
    bool keyBeginDown[350] = {0};
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
    int mouseDownCount = 0;
    bool mouseDragging = 0;
    
}

namespace Pancake {

    void KeyListener::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

        if (key < 0 || key > 349) {return;}

        if (action == GLFW_PRESS) {
            keyPressCount++;
            keyDown[key] = true;
            keyBeginDown[key] = true;
        }

        else if (action == GLFW_RELEASE) {
            keyPressCount--;
            keyDown[key] = false;
            keyBeginDown[key] = false;
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

        if (keyBeginDown[key]) {
            keyBeginDown[key] = false;
            return true;
        }

        return false;
    }

    void MouseListener::calcOrthoX() {

        float currentX = ((float) x / Window::getWidth()) * 2.0f - 1.0f;
        Camera* camera = Window::getScene()->getCamera();
        worldX = camera->getPosition().x + (currentX / 2.0f) * (camera->getProjectionSize().x / camera->getZoom());

    }

    void MouseListener::calcOrthoY() {

        float currentY = ((float) y / Window::getHeight()) * 2.0f - 1.0f;
        Camera* camera = Window::getScene()->getCamera();
        worldY = camera->getPosition().y + (currentY / 2.0f) * (camera->getProjectionSize().y / camera->getZoom());

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
            mouseDownCount++;
            mouseDown[button] = true;
            mouseBeginDown[button] = true;
        }

        else if (action == GLFW_RELEASE) {
            mouseDownCount--;
            mouseDown[button] = false;
            mouseBeginDown[button] = false;
            mouseDragging = false;
        }

    }

    void MouseListener::mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
        scrollX = xOffset;
        scrollY = yOffset;
    }

    bool MouseListener::isMouseDown(int button) {
        if (button < 0 || button > 9) {return false;}
        return mouseDown[button];
    }

    bool MouseListener::isMouseDown() {
        return mouseDownCount > 0;
    }

    bool MouseListener::isMouseBeginDown(int button) {

        if (button < 0 || button > 8) {return false;}

        if (mouseBeginDown[button]) {
            mouseBeginDown[button] = false;
            return true;
        }

        return false;
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

    void MouseListener::endFrame() {

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