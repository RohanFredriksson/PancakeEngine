#include <math.h>
#include <glm/geometric.hpp>
#include "components/cameracontroller.hpp"
#include "core/window.hpp"

CameraController::CameraController() : Component() {

    this->moving = false;
    this->initialPosition = glm::vec2(0.0f, 0.0f);
    this->finalPosition = glm::vec2(0.0f, 0.0f);
    this->normal = glm::vec2(0.0f, 0.0f);
    this->timeCurrent = 0.0f;
    this->timeTotal = 0.0f;
    this->timeHalf = 0.0f;
    this->distance = 0.0f;
    this->maxVelocity = 0.0f;

}

void CameraController::update(float dt) {

    Camera* camera = Window::getScene()->getCamera();

    // If the camera is moving, smoothly move the camera.
    if (this->moving) {

        // If the camera has moved to the end, set the position exactly
        if (this->timeCurrent >= this->timeTotal) {
            this->moving = false;
            camera->setPosition(this->finalPosition);
        }

        // If the camera is still moving, calculate its new position
        else {

            // Calculate the current velocity of coefficient.
            float tmp = (this->timeCurrent - this->timeHalf) / this->timeHalf;
            float velocityFactor = sqrtf(1 - (tmp * tmp));

            // Displace the camera.
            vec2 currentDisplacement = this->normal * this->maxVelocity * velocityFactor * dt;
            camera->addPosition(currentDisplacement);

        }
        this->timeCurrent += dt;
    }

}

void CameraController::moveTo(vec2 to, float t) {

    Camera* camera = Window::getScene()->getCamera();
    this->initialPosition = camera->getPosition();
    this->finalPosition = to;
    this->distance = glm::distance(this->initialPosition, this->finalPosition);

    // If the camera, doesn't need to move, don't move it.
    if (this->distance == 0.0f) {return;}

    this->normal = glm::normalize(this->finalPosition - this->initialPosition);
    this->timeCurrent = 0.0f;
    this->timeTotal = t;
    this->timeHalf = t/2;

    // If the time required to move is zero, move the camera immediately.
    if (this->timeTotal <= 0.0f) {
        camera->setPosition(to);
        return;
    }

    // Define the max velocity, and raise the moving flag.
    this->maxVelocity = (2 * this->distance) / (M_PI * this->timeHalf);
    this->moving = true;

}