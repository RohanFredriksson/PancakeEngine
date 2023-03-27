#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

using std::vector;
using glm::vec2;
using json = nlohmann::json;

class Component;

class Entity {

    private:

        int id;
        vector<Component*> components;
        vector<Component*> newComponents;
        vector<int> deadComponentIds;
        vec2 position;
        vec2 size;
        float rotation;
        bool dead;

        void init(int id, vec2 position, vec2 size, float radians);

    public:

        Entity(vec2 position, vec2 size, float radians);
        ~Entity();
        void update(float dt);
        json serialise();
        void onCollision(Component* with);
        void kill();

        // Getter Methods.
        int getId();
        vector<Component*> getComponents();
        vector<Component*> getNewComponents();
        vector<int> getDeadComponentIds();
        vec2 getPosition();
        vec2 getSize();
        float getRotation();
        bool isDead();

        // Setter Methods.
        void setPosition(vec2 position);
        void setSize(vec2 size);
        void setRotation(float radians);

        // Adder Methods.
        void addPosition(vec2 position);
        void addSize(vec2 size);
        void addRotation(float radians);
        void addRotationAround(float radians, vec2 around);

        // Component Methods
        void addComponent(Component* component);
        void clearNewComponents();
        void clearDeadComponentIds();

};

#include "pancake/core/component.hpp"