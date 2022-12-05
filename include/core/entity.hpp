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

    public:

        Entity(vec2 position, vec2 size, float rotation);
        ~Entity();

        void update(float dt);
        void kill();
    
        // Getters
        int getId();
        vector<Component*> getComponents();
        vector<Component*> getNewComponents();
        vec2 getPosition();
        vec2 getSize();
        float getRotation();
        bool isDead();

        // Setters
        void setPosition(vec2 position);
        void setSize(vec2 size);
        void setRotation(float rotation);

        // Adders
        void addPosition(vec2 position);
        void addSize(vec2 size);
        void addRotation(float rotation);

        void clearNewComponents();

};

#include "core/component.hpp"