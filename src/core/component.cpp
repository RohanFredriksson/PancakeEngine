#include <algorithm>
#include <unordered_map>
#include "pancake/core/component.hpp"
#include "pancake/core/entity.hpp"

using std::unordered_map;

namespace Pancake {

    namespace {
        int nextId = 0;
    }

    void Component::init(int id, string type, bool load) {

        this->id = id;
        this->type = type;
        this->entity = nullptr;
        this->serialisable = true;
        this->imguiable = true;
        this->dead = false;

        if (load) {nextId = std::max(nextId, id+1);}
        else {nextId++;}

    }

    Component::Component(string type) {
        this->init(nextId, type, false);
    }

    Component::~Component() {
        if (!this->dead) {this->end();}
    }

    void Component::start() {
        
    }

    void Component::end() {

    }

    void Component::update(float dt) {

    }

    json Component::serialise() {
        json j;
        j.emplace("id", this->id);
        j.emplace("type", this->type);
        j.emplace("imguiable", this->imguiable);
        return j;
    }

    bool Component::load(json j) {
        
        // Required attributes.
        if (!j.contains("id") || !j["id"].is_number_integer()) {return false;}
        if (!j.contains("type") || !j["type"].is_string()) {return false;}
        this->init(j["id"], j["type"], true);

        // Optional attributes.
        if (j.contains("imguiable") && j["imguiable"].is_boolean()) {this->setImguiable(j["imguiable"]);}

        return true;
    }

    void Component::imgui() {
        
    }

    void Component::kill() {
        if (this->dead) {return;}
        this->end();
        this->dead = true;
    }

    int Component::getId() {
        return this->id;
    }

    string Component::getType() {
        return this->type;
    }

    Entity* Component::getEntity() {
        return this->entity;
    }

    bool Component::isSerialisable() {
        return this->serialisable;
    }

    bool Component::isImguiable() {
        return this->imguiable;
    }

    bool Component::isDead() {
        return this->dead;
    }

    void Component::setId(int id) {
        this->id = id;
    }

    void Component::setEntity(Entity* entity) {
        this->entity = entity;
    }

    void Component::setSerialisable(bool serialisable) {
        this->serialisable = serialisable;
    }

    void Component::setImguiable(bool imguiable) {
        this->imguiable = imguiable;
    }

    TransformableComponent::TransformableComponent(string type) : Component(type) {
        this->positionOffset = glm::vec2(0.0f, 0.0f);
        this->sizeScale = glm::vec2(1.0f, 1.0f);
        this->rotationOffset = 0.0f;
    }

    json TransformableComponent::serialise() {
        json j = this->Component::serialise();
        j.emplace("positionOffset", json::array());
        j["positionOffset"].push_back(this->positionOffset.x);
        j["positionOffset"].push_back(this->positionOffset.y);
        j.emplace("sizeScale", json::array());
        j["sizeScale"].push_back(this->sizeScale.x);
        j["sizeScale"].push_back(this->sizeScale.y);
        j.emplace("rotationOffset", this->rotationOffset);
        return j;
    }

    bool TransformableComponent::load(json j) {

        if (!this->Component::load(j)) {return false;}

        if (!j.contains("positionOffset") || !j["positionOffset"].is_array()) {return false;}
        if (j["positionOffset"].size() != 2) {return false;}
        if (!j["positionOffset"][0].is_number()) {return false;}
        if (!j["positionOffset"][1].is_number()) {return false;}

        if (!j.contains("sizeScale") || !j["sizeScale"].is_array()) {return false;}
        if (j["sizeScale"].size() != 2) {return false;}
        if (!j["sizeScale"][0].is_number()) {return false;}
        if (!j["sizeScale"][1].is_number()) {return false;}

        if (!j.contains("rotationOffset") || !j["rotationOffset"].is_number()) {return false;}

        this->setPositionOffset(vec2(j["positionOffset"][0], j["positionOffset"][1]));
        this->setSizeScale(vec2(j["sizeScale"][0], j["sizeScale"][1]));
        this->setRotationOffset(j["rotationOffset"]);

        return true;

    }

    void TransformableComponent::imgui() {

        Component::imgui();

        // Position Offset
        vec2 position = vec2(this->positionOffset.x, this->positionOffset.y);
        ImGui::Text("Position Offset:");
        ImGui::SameLine();
        if (ImGui::DragFloat2("##PositionOffset", &position[0])) {this->setPositionOffset(position);}

        // Size Scale
        vec2 size = vec2(this->sizeScale.x, this->sizeScale.y);
        ImGui::Text("Size Scale:     ");
        ImGui::SameLine();
        if (ImGui::DragFloat2("##SizeScale", &size[0])) {this->setSizeScale(size);}

        // Rotation Offset
        float rotation = this->rotationOffset;
        ImGui::Text("Rotation Offset:");
        ImGui::SameLine();
        if (ImGui::DragFloat("##RotationOffset", &rotation)) {this->setRotationOffset(rotation);}

    }

    vec2 TransformableComponent::getPosition() {
        return this->getEntity()->getPosition() + this->positionOffset;
    }

    vec2 TransformableComponent::getSize() {
        return this->getEntity()->getSize() * this->sizeScale;
    }

    float TransformableComponent::getRotation() {
        return this->getEntity()->getRotation() + this->rotationOffset;
    }

    vec2 TransformableComponent::getPositionOffset() {
        return this->positionOffset;
    }

    vec2 TransformableComponent::getSizeScale() {
        return this->sizeScale;
    }

    float TransformableComponent::getRotationOffset() {
        return this->rotationOffset;
    }

    void TransformableComponent::setPositionOffset(vec2 offset) {
        this->positionOffset = offset;
    }

    void TransformableComponent::setPositionOffset(float x, float y) {
        this->setPositionOffset(vec2(x, y));
    }

    void TransformableComponent::setSizeScale(vec2 scale) {
        this->sizeScale = scale;
    }

    void TransformableComponent::setSizeScale(float w, float h) {
        this->setSizeScale(vec2(w, h));
    }

    void TransformableComponent::setRotationOffset(float offset) {
        this->rotationOffset = offset;
    }

    void TransformableComponent::addPositionOffset(vec2 offset) {
        this->positionOffset += offset;
    }

    void TransformableComponent::addPositionOffset(float x, float y) {
        this->addPositionOffset(vec2(x, y));
    }

    void TransformableComponent::addSizeScale(vec2 scale) {
        this->sizeScale += scale;
    }

    void TransformableComponent::addSizeScale(float w, float h) {
        this->addSizeScale(vec2(w, h));
    }

    void TransformableComponent::addRotationOffset(float offset) {
        this->rotationOffset += offset;
    }

}