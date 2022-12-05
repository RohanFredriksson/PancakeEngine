#pragma once

#include <vector>
#include <glm/glm.hpp>

using std::vector;
using glm::vec2;

class Component;

class Entity {

    private:

        int id;
        vector<Component*> components;
        vector<Component*> newComponents;
        vec2 position;
        vec2 size;
        float rotation;
        bool dead;

        void init(int id, vec2 position, vec2 size, float rotation);

    public:

        Entity(vec2 position, vec2 size, float rotation);
        ~Entity();

        void update(float dt);
        void kill();

        // Getter Methods.
        int getId();
        vector<Component*> getComponents();
        vector<Component*> getNewComponents();
        vec2 getPosition();
        vec2 getSize();
        float getRotation();
        bool isDead();

        // Setter Methods.
        void setPosition(vec2 position);
        void setSize(vec2 size);
        void setRotation(float rotation);

        // Adder Methods.
        void addPosition(vec2 position);
        void addSize(vec2 size);
        void addRotation(float rotation);

        // Component Methods
        void addComponent(Component* component);
        void clearNewComponents();

};

#include "core/component.hpp"