#include "core/scene.hpp"
#include <string>
#include <vector>
#include <glm/glm.hpp>

using glm::vec2;

Scene::Scene(string name) {
    this->name = name;
    this->camera = new Camera(vec2(0.0f, 0.0f), vec2(12.0f, 12.0f), 1.0f);
}

Scene::~Scene() {
    delete this->camera;
}

void Scene::update(float dt) {

}

void Scene::render() {

}