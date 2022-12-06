#include <glad/glad.h>
#include "core/listener.hpp"
#include "core/camera.hpp"
#include "core/window.hpp"

namespace {

    bool keyPressed[350] = {0};
    bool keyBeginPress[350] = {0};

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
    bool mouseButtonDown[9] = {0};
    bool mouseButtonBeginDown[9] = {0};
    bool isDragging = 0;
    int mouseButtonsDown = 0;

}

void KeyListener::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (action == GLFW_PRESS) {
        keyPressed[key] = true;
        keyBeginPress[key] = true;
    }

    else if (action == GLFW_RELEASE) {
        keyPressed[key] = false;
        keyBeginPress[key] = false;
    }

}

bool KeyListener::isKeyPressed(int key) {
    return keyPressed[key];
}

bool KeyListener::isKeyBeginPress(int key) {

    bool result = keyBeginPress[key];
    if (result) {
        keyBeginPress[key] = false;
    }

    return result;

}

void WindowListener::resizeCallback(GLFWwindow* window, int screenWidth, int screenHeight) {

    Window::setWidth(screenWidth);
    Window::setHeight(screenHeight);
    Window::resetFramebuffers();
    Window::getScene()->getCamera()->adjustProjection();
    glViewport(0, 0, screenWidth, screenHeight);

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

    if (mouseButtonsDown > 0) {
        isDragging = true;
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

    if (action == GLFW_PRESS) {

        mouseButtonsDown++;

        if (button < 9) {
            mouseButtonDown[button] = true;
            mouseButtonBeginDown[button] = true;
        }

    }

    else if (action == GLFW_RELEASE) {

        mouseButtonsDown--;

        if (button < 9) {
            mouseButtonDown[button] = false;
            mouseButtonBeginDown[button] = false;
            isDragging = false;
        }

    }

}

void MouseListener::mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    scrollX = xOffset;
    scrollY = yOffset;
}

bool MouseListener::isMouseButtonDown(int button) {

    if (button < 9) {
        return mouseButtonDown[button];
    }
    return 0;

}

bool MouseListener::isMouseButtonBeginDown(int button) {

    if (button < 9) {
        bool result = mouseButtonBeginDown[button];
        if (result) {
            mouseButtonBeginDown[button] = false;
        }
        return result;
    }
    return false;

}

bool MouseListener::isMouseDragging() {
    return isDragging;
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