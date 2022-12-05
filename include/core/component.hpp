#pragma once

#include <vector>
#include <glm/glm.hpp>

using std::vector;
using glm::vec2;

class Entity;

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

        virtual void update(float dt);
        void kill();
        
        // Getters
        int getId();
        Entity* getEntity();
        vec2 getPosition();
        vec2 getSize();
        float getRotation();
        vec2 getPositionOffset();
        vec2 getSizeScale();
        float getRotationOffset();
        bool isDead();

        // Setters
        void setEntity(Entity* entity);
        void setPositionOffset(vec2 offset);
        void setSizeScale(vec2 scale);
        void setRotationOffset(float offset);

        // Adders
        void addPositionOffset(vec2 offset);
        void addSizeScale(vec2 scale);
        void addRotationOffset(float offset);

};

#include "core/entity.hpp"