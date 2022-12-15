#include "core/camera.hpp"
#include "window/window.hpp"
#include <glm/gtx/transform.hpp>

using glm::vec3;

Camera::Camera(vec2 position, vec2 projectionSize, float zoom) {

    this->position = position;
    this->projectionSize = projectionSize;
    this->zoom = zoom;
    
    this->adjustProjection();

}

void Camera::adjustProjection() {
    this->projectionSize.x = this->projectionSize.y * Window::getAspectRatio();
    this->projection = glm::ortho(0.0f, this->projectionSize.x / this->zoom, 0.0f, this->projectionSize.y / this->zoom, 0.0f, 100.0f);
    this->inverseProjection = glm::inverse(this->projection);
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