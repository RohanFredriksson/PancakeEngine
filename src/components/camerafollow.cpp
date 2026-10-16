#include <cmath>
#include <imgui.h>

#include "pancake/components/camerafollow.hpp"
#include "pancake/core/window.hpp"

namespace Pancake {

    namespace {

        bool readVec2(json& j, const char* key, glm::vec2* out) {
            if (!j.contains(key) || !j[key].is_array() || j[key].size() != 2) {return false;}
            if (!j[key][0].is_number() || !j[key][1].is_number()) {return false;}
            *out = glm::vec2(j[key][0], j[key][1]);
            return true;
        }

        json writeVec2(glm::vec2 v) {
            return json::array({v.x, v.y});
        }

        // Moves value just enough to keep target within halfSize of it.
        float applyDeadZone(float value, float target, float halfSize) {
            float d = target - value;
            if (d > halfSize) {return target - halfSize;}
            if (d < -halfSize) {return target + halfSize;}
            return value;
        }

        // Keeps a view of the given half size inside [min, max], centring it when it doesn't fit.
        float clampView(float centre, float halfView, float min, float max) {
            if (max - min <= 2.0f * halfView) {return 0.5f * (min + max);}
            return std::fmin(std::fmax(centre, min + halfView), max - halfView);
        }

    }

    CameraFollow::CameraFollow() : Component("CameraFollow") {
        this->target = -1;
        this->offset = glm::vec2(0.0f, 0.0f);
        this->smoothing = 0.0f;
        this->deadZone = glm::vec2(0.0f, 0.0f);
        this->bounded = false;
        this->boundsMin = glm::vec2(0.0f, 0.0f);
        this->boundsMax = glm::vec2(0.0f, 0.0f);
        this->snapNext = true;
    }

    void CameraFollow::start() {
        this->snapNext = true;
    }

    void CameraFollow::update(float dt) {
        Scene* scene = Window::getScene();
        if (scene == nullptr) {return;}
        Entity* followed = this->target < 0 ? this->getEntity() : scene->getEntity(this->target);
        if (followed == nullptr || followed->isDead()) {return;}
        this->apply(scene->getCamera(), followed, dt);
    }

    void CameraFollow::apply(Camera* camera, Entity* followed, float dt) {

        glm::vec2 position = camera->getPosition();
        glm::vec2 goal = followed->getPosition() + this->offset;

        // Only move when the target leaves the dead zone, and then only as far as its edge.
        glm::vec2 desired;
        desired.x = applyDeadZone(position.x, goal.x, this->deadZone.x);
        desired.y = applyDeadZone(position.y, goal.y, this->deadZone.y);

        // Close the gap exponentially, so the result doesn't depend on the frame rate.
        if (this->snapNext || this->smoothing <= 0.0f) {position = desired;}
        else {position += (desired - position) * (1.0f - std::exp(-dt / this->smoothing));}
        this->snapNext = false;

        if (this->bounded) {
            glm::vec2 halfView = 0.5f * camera->getProjectionSize() / camera->getZoom();
            position.x = clampView(position.x, halfView.x, this->boundsMin.x, this->boundsMax.x);
            position.y = clampView(position.y, halfView.y, this->boundsMin.y, this->boundsMax.y);
        }

        camera->setPosition(position);

    }

    json CameraFollow::serialise() {
        json j = this->Component::serialise();
        j.emplace("target", this->target);
        j.emplace("offset", writeVec2(this->offset));
        j.emplace("smoothing", this->smoothing);
        j.emplace("deadZone", writeVec2(this->deadZone));
        j.emplace("bounded", this->bounded);
        j.emplace("boundsMin", writeVec2(this->boundsMin));
        j.emplace("boundsMax", writeVec2(this->boundsMax));
        return j;
    }

    bool CameraFollow::load(json j) {

        if (!this->Component::load(j)) {return false;}
        if (!j.contains("target") || !j["target"].is_number_integer()) {return false;}
        if (!j.contains("smoothing") || !j["smoothing"].is_number()) {return false;}
        if (!j.contains("bounded") || !j["bounded"].is_boolean()) {return false;}

        glm::vec2 offset, deadZone, boundsMin, boundsMax;
        if (!readVec2(j, "offset", &offset)) {return false;}
        if (!readVec2(j, "deadZone", &deadZone)) {return false;}
        if (!readVec2(j, "boundsMin", &boundsMin)) {return false;}
        if (!readVec2(j, "boundsMax", &boundsMax)) {return false;}

        this->target = j["target"];
        this->offset = offset;
        this->smoothing = j["smoothing"];
        this->deadZone = deadZone;
        this->bounded = j["bounded"];
        this->boundsMin = boundsMin;
        this->boundsMax = boundsMax;
        return true;

    }

    void CameraFollow::imgui() {

        ImGui::Text("Target:         ");
        ImGui::SameLine();
        ImGui::InputInt("##Target", &this->target);

        ImGui::Text("Offset:         ");
        ImGui::SameLine();
        ImGui::DragFloat2("##Offset", &this->offset[0], 0.05f);

        ImGui::Text("Smoothing:      ");
        ImGui::SameLine();
        ImGui::DragFloat("##Smoothing", &this->smoothing, 0.01f, 0.0f, 10.0f);

        ImGui::Text("Dead Zone:      ");
        ImGui::SameLine();
        if (ImGui::DragFloat2("##DeadZone", &this->deadZone[0], 0.05f)) {this->setDeadZone(this->deadZone);}

        ImGui::Checkbox("Bounded", &this->bounded);
        if (this->bounded) {
            ImGui::Text("Bounds Min:     ");
            ImGui::SameLine();
            ImGui::DragFloat2("##BoundsMin", &this->boundsMin[0], 0.1f);
            ImGui::Text("Bounds Max:     ");
            ImGui::SameLine();
            ImGui::DragFloat2("##BoundsMax", &this->boundsMax[0], 0.1f);
        }

    }

    int CameraFollow::getTarget() {
        return this->target;
    }

    glm::vec2 CameraFollow::getOffset() {
        return this->offset;
    }

    float CameraFollow::getSmoothing() {
        return this->smoothing;
    }

    glm::vec2 CameraFollow::getDeadZone() {
        return this->deadZone;
    }

    bool CameraFollow::hasBounds() {
        return this->bounded;
    }

    glm::vec2 CameraFollow::getBoundsMin() {
        return this->boundsMin;
    }

    glm::vec2 CameraFollow::getBoundsMax() {
        return this->boundsMax;
    }

    CameraFollow* CameraFollow::setTarget(Entity* entity) {
        this->target = entity == nullptr ? -1 : entity->getId();
        this->snapNext = true;
        return this;
    }

    CameraFollow* CameraFollow::setTarget(int entityId) {
        this->target = entityId < 0 ? -1 : entityId;
        this->snapNext = true;
        return this;
    }

    CameraFollow* CameraFollow::setOffset(glm::vec2 offset) {
        this->offset = offset;
        return this;
    }

    CameraFollow* CameraFollow::setSmoothing(float seconds) {
        this->smoothing = std::fmax(seconds, 0.0f);
        return this;
    }

    CameraFollow* CameraFollow::setDeadZone(glm::vec2 halfSize) {
        this->deadZone = glm::max(halfSize, glm::vec2(0.0f, 0.0f));
        return this;
    }

    CameraFollow* CameraFollow::setBounds(glm::vec2 min, glm::vec2 max) {
        this->bounded = true;
        this->boundsMin = glm::min(min, max);
        this->boundsMax = glm::max(min, max);
        return this;
    }

    CameraFollow* CameraFollow::clearBounds() {
        this->bounded = false;
        return this;
    }

    CameraFollow* CameraFollow::snap() {
        this->snapNext = true;
        return this;
    }

}
