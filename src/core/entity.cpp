#include <cmath>
#include <deque>
#include <algorithm>
#include <unordered_set>
#include <imgui.h>

#include "pancake/core/entity.hpp"
#include "pancake/core/component.hpp"
#include "pancake/core/factory.hpp"

using std::deque;

namespace Pancake {

    namespace {
        int nextId = 0;
    }

    void Entity::init(int id, vec2 position, vec2 size, float radians, bool load) {

        this->id = id;
        this->started = false;
        this->position = position;
        this->size = size;
        this->rotation = radians;
        this->serialisable = true;
        this->dead = false;

        if (load) {nextId = std::max(nextId, id+1);}
        else {nextId++;}

    }

    Entity::Entity(vec2 position, vec2 size, float radians) {
        this->init(nextId, position, size, radians, false);
    }

    Entity::Entity() {
        this->init(nextId, vec2(0.0f, 0.0f), vec2(1.0f, 1.0f), 0.0f, false);
    }

    Entity::~Entity() {

        // Delete all the components.
        for (Component* c: this->components) {c->kill();}
        for (Component* c: this->components) {delete c;}

    }

    void Entity::start() {
        this->started = true;
        for (Component* c : this->components) {
            c->start();
        }
    }

    void Entity::update(float dt) {

        // Update all the components.
        deque<int> deadIndices;
        int index = 0;
        for (Component* c : this->components) {
            if (!c->isDead()) {c->update(dt);} 
            else {deadIndices.push_front(index);}
            index++;
        }

        // Delete all dead elements
        for (int i : deadIndices) {
            Component* c = this->components[i];
            this->deadComponentIds.push_back(c->getId());
            this->components.erase(this->components.begin() + i);
            delete c;
        }
        deadIndices.clear();

    }

    json Entity::serialise() {
        
        json j;
        j.emplace("id", this->id);

        j.emplace("position", json::array());
        j["position"].push_back(this->position.x);
        j["position"].push_back(this->position.y);
        
        j.emplace("size", json::array());
        j["size"].push_back(this->size.x);
        j["size"].push_back(this->size.y);
        
        j.emplace("rotation", this->rotation);
        
        j.emplace("components", json::array());
        for (Component* c : this->components) {
            if (c->isSerialisable() && !c->isDead()) {j["components"].push_back(c->serialise());}
        }

        return j;
    }

    Entity* Entity::load(json j) {

        if (!j.contains("id") || !j["id"].is_number_integer()) {return nullptr;}

        if (!j.contains("position") || !j["position"].is_array()) {return nullptr;}
        if (j["position"].size() != 2) {return nullptr;}
        if (!j["position"][0].is_number()) {return nullptr;}
        if (!j["position"][1].is_number()) {return nullptr;}

        if (!j.contains("size") || !j["size"].is_array()) {return nullptr;}
        if (j["size"].size() != 2) {return nullptr;}
        if (!j["size"][0].is_number()) {return nullptr;}
        if (!j["size"][1].is_number()) {return nullptr;}

        if (!j.contains("rotation") || !j["rotation"].is_number()) {return nullptr;}

        Entity* e = new Entity();
        e->init(j["id"], vec2(j["position"][0], j["position"][1]), vec2(j["size"][0], j["size"][1]), j["rotation"], true);

        if (j.contains("components") && j["components"].is_array()) {
            for (auto element : j["components"]) {
                if (element.is_object()) {
                    if (!element.contains("type") || !element["type"].is_string()) {continue;}
                    Component* c = FACTORY(Component).create(element["type"]);
                    if (c == nullptr) {continue;}
                    if (!c->load(element)) {delete c; continue;}
                    else {e->addComponent(c);}
                }
            }
        }

        return e;
    }

    void Entity::imgui() {

        // Position 
        vec2 position = vec2(this->position.x, this->position.y);
        ImGui::Text("Position:       ");
        ImGui::SameLine();
        if (ImGui::DragFloat2("##Position", &position[0])) {this->setPosition(position);}

        // Size
        vec2 size = vec2(this->size.x, this->size.y);
        ImGui::Text("Size:           ");
        ImGui::SameLine();
        if (ImGui::DragFloat2("##Size", &size[0])) {this->setSize(size);}

        // Rotation
        float rotation = this->rotation;
        ImGui::Text("Rotation:       ");
        ImGui::SameLine();
        if (ImGui::DragFloat("##Rotation", &rotation)) {this->setRotation(rotation);}

        for (Component* c : this->components) {

            // If the component should not have a gui, don't render it.
            if (!c->isImguiable()) {continue;}

            // Create a widget for each component that is supposed to have it.
            ImGui::Spacing();
            string id = c->getType() + " " + std::to_string(c->getId());
            ImGui::PushID(id.c_str());
            if (ImGui::CollapsingHeader(id.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                c->imgui();
            }
            ImGui::PopID();

        }

    }

    void Entity::kill() {
        if (this->dead) {return;}
        for (Component* c: this->components) {c->kill();}
        this->dead = true;
    }

    int Entity::getId() {
        return this->id;
    }

    vector<Component*> Entity::getComponents() {
        return this->components;
    }

    vector<Component*> Entity::getNewComponents() {
        return this->newComponents;
    }

    vector<int> Entity::getDeadComponentIds() {
        return this->deadComponentIds;
    }

    vec2 Entity::getPosition() {
        return this->position;
    }

    vec2 Entity::getSize() {
        return this->size;
    }

    float Entity::getRotation() {
        return this->rotation;
    }

    bool Entity::isSerialisable() {
        return this->serialisable;
    }

    bool Entity::isDead() {
        return this->dead;
    }

    Component* Entity::getComponent(string type) {
        for (Component* c : this->components) {
            if (c->getType() == type) {
                return c;
            }
        }
        return nullptr;
    }

    void Entity::setPosition(vec2 position) {
        this->position = position;
    }

    void Entity::setSize(vec2 size) {
        this->size = size;
    }

    void Entity::setRotation(float radians) {
        this->rotation = radians;
    }

    void Entity::setSerialisable(bool serialisable) {
        this->serialisable = serialisable;
    }

    void Entity::addPosition(vec2 position) {
        this->position += position;
    }

    void Entity::addSize(vec2 size) {
        this->size += size;
    }

    void Entity::addRotation(float radians) {
        
        // If we don't need to perform expensive computations, don't.
        if (radians == 0.0f) {return;}

        // Rotate the entity
        this->rotation += radians;

        // Get required values to rotate all component offsets.
        float rCos = cosf(radians);
        float rSin = sinf(radians);

        // Rotate all transformable components.
        for (Component* c : this->components) {

            TransformableComponent* t = dynamic_cast<TransformableComponent*>(c);
            if (t != nullptr) {
                vec2 offset = t->getPositionOffset();
                float x = (offset.x * rCos) - (offset.y * rSin);
                float y = (offset.x * rSin) + (offset.y * rCos);
                t->setPositionOffset(vec2(x, y));
            }

        }

    }

    void Entity::addRotationAround(float radians, vec2 around) {

        // If we don't need to perform expensive computations, don't.
        if (radians == 0.0f) {return;}

        if (around.x == 0.0f && around.y == 0.0f) {
            this->addRotation(radians);
            return;
        }
        
        float x = this->position.x - around.x;
        float y = this->position.y - around.y;
        float rCos = cosf(radians);
        float rSin = sinf(radians);

        // Rotate the entity around the position.
        this->position.x = around.x + ((x * rCos) - (y * rSin));
        this->position.y = around.y + ((x * rSin) + (y * rCos));
        this->rotation += radians;

        // Rotate all position offsets of components
        for (Component* c : this->components) {

            TransformableComponent* t = dynamic_cast<TransformableComponent*>(c);
            if (t != nullptr) {
                vec2 offset = t->getPositionOffset();
                float x = (offset.x * rCos) - (offset.y * rSin);
                float y = (offset.x * rSin) + (offset.y * rCos);
                t->setPositionOffset(vec2(x, y));
            }

        }

    }

    void Entity::addComponent(Component* component) {
        component->setEntity(this);
        this->components.push_back(component);
        this->newComponents.push_back(component);
        if (this->started) {component->start();}
    }

    void Entity::clearNewComponents() {
        this->newComponents.clear();
    }

    void Entity::clearDeadComponentIds() {
        this->deadComponentIds.clear();
    }

}