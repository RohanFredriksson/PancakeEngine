#include "pancake/core/camera.hpp"
#include "pancake/window/window.hpp"
#include <glm/gtx/transform.hpp>

using glm::vec3;

Camera::Camera(vec2 position, vec2 projectionSize, float zoom) {

    this->position = position;
    this->projectionSize = projectionSize;
    this->zoom = zoom;

    this->moving = false;
    this->initialPosition = glm::vec2(0.0f, 0.0f);
    this->finalPosition = glm::vec2(0.0f, 0.0f);
    this->normal = glm::vec2(0.0f, 0.0f);
    this->timeCurrent = 0.0f;
    this->timeTotal = 0.0f;
    this->timeHalf = 0.0f;
    this->distance = 0.0f;
    this->maxVelocity = 0.0f;
    
    this->adjustProjection();

}

void Camera::adjustProjection() {
    this->projectionSize.x = this->projectionSize.y * Window::getAspectRatio();
    this->projection = glm::ortho(0.0f, this->projectionSize.x / this->zoom, 0.0f, this->projectionSize.y / this->zoom, 0.0f, 100.0f);
    this->inverseProjection = glm::inverse(this->projection);
}

void Camera::update(float dt) {

    // If the camera is moving, smoothly move the camera.
    if (this->moving) {

        // If the camera has moved to the end, set the position exactly
        if (this->timeCurrent >= this->timeTotal) {
            this->moving = false;
            this->position = this->finalPosition;
        }

        // If the camera is still moving, calculate its new position
        else {

            // Calculate the current velocity of coefficient.
            float tmp = (this->timeCurrent - this->timeHalf) / this->timeHalf;
            float velocityFactor = sqrtf(1 - (tmp * tmp));

            // Displace the camera.
            vec2 currentDisplacement = this->normal * this->maxVelocity * velocityFactor * dt;
            this->position += currentDisplacement;

        }
        this->timeCurrent += dt;
    }

}

void Camera::move(vec2 to, float t) {

    // If the time required to move is zero, move the camera immediately.
    if (t <= 0.0f) {
        this->position = to;
        this->moving = false;
        return;
    }

    this->initialPosition = this->position;
    this->finalPosition = to;
    this->distance = glm::distance(this->initialPosition, this->finalPosition);

    // If the camera, doesn't need to move, don't move it.
    if (this->distance == 0.0f) {return;}

    this->normal = glm::normalize(this->finalPosition - this->initialPosition);
    this->timeCurrent = 0.0f;
    this->timeTotal = t;
    this->timeHalf = t/2;

    // Define the max velocity, and raise the moving flag.
    this->maxVelocity = (2 * this->distance) / (M_PI * this->timeHalf);
    this->moving = true;

}

json Camera::serialise() {
    
    json j;
    
    j.emplace("position", json::array());
    j["position"].push_back(this->position.x);
    j["position"].push_back(this->position.y);

    j.emplace("projectionSize", json::array());
    j["projectionSize"].push_back(this->projectionSize.x);
    j["projectionSize"].push_back(this->projectionSize.y);

    j.emplace("zoom", this->zoom);
    j.emplace("moving", this->moving);
    
    j.emplace("initialPosition", json::array());
    j["initialPosition"].push_back(this->initialPosition.x);
    j["initialPosition"].push_back(this->initialPosition.y);

    j.emplace("finalPosition", json::array());
    j["finalPosition"].push_back(this->finalPosition.x);
    j["finalPosition"].push_back(this->finalPosition.y);

    j.emplace("normal", json::array());
    j["normal"].push_back(this->normal.x);
    j["normal"].push_back(this->normal.y);

    j.emplace("timeCurrent", this->timeCurrent);
    j.emplace("timeTotal", this->timeTotal);
    j.emplace("timeHalf", this->timeHalf);
    j.emplace("distance", this->distance);
    j.emplace("maxVelocity", this->maxVelocity);

    return j;
    
}

vec2 Camera::getPosition() {
    return this->position;
}

vec2 Camera::getProjectionSize() {
    return this->projectionSize;
}

mat4 Camera::getProjection() {
    return this->projection;
}

mat4 Camera::getView() {

    vec3 cameraPosition(this->position.x - this->projectionSize.x / (2 * this->zoom), this->position.y - this->projectionSize.y / (2 * this->zoom), 20.0f);
    vec3 cameraFront(this->position.x - this->projectionSize.x / (2 * this->zoom), this->position.y - this->projectionSize.y / (2 * this->zoom), -1.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);

    this->view = glm::lookAt(cameraPosition, cameraFront, cameraUp);
    this->inverseView = glm::inverse(this->view);
    return this->view;

}

mat4 Camera::getInverseProjection() {
    return this->inverseProjection;
}

mat4 Camera::getInverseView() {
    this->getView();
    return this->inverseView;
}

float Camera::getZoom() {
    return this->zoom;
}

void Camera::setPosition(vec2 position) {
    this->position = position;
}

void Camera::setProjectionSize(vec2 projectionSize) {
    this->projectionSize = projectionSize;
}

void Camera::setZoom(float zoom) {
    this->zoom = zoom;
}

void Camera::addPosition(vec2 position) {
    this->position += position;
}

void Camera::addZoom(float zoom) {
    this->zoom += zoom;
}

void Camera::scaleZoom(float scale) {
    this->zoom *= scale;
}