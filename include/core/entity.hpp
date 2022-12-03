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
    
        bool isDead();

        vector<Component*> getNewComponents();
        void clearNewComponents();

};

class Component {

    private:

        int id;
        Entity* entity;
        vec2 positionOffset;
        vec2 sizeScale;
        float rotationOffset;
        bool dead;

    public:

        Component();
        ~Component();

        void update(float dt);
        void kill();
        
        bool isDead();

};