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
        bool dead;

    public:

        Component();
        virtual ~Component();
        virtual void update(float dt);
        virtual void onCollision(Component* with);
        void kill();
        
        int getId();
        Entity* getEntity();
        bool isDead();
        
        void setId(int id);
        void setEntity(Entity* entity);
    
};

class TransformableComponent : public Component {

    private:

        vec2 positionOffset;
        vec2 sizeScale;
        float rotationOffset;

    public:

        TransformableComponent();

        vec2 getPosition();
        vec2 getSize();
        float getRotation();
        vec2 getPositionOffset();
        vec2 getSizeScale();
        float getRotationOffset();

        void setPositionOffset(vec2 offset);
        void setSizeScale(vec2 scale);
        void setRotationOffset(float offset);

        void addPositionOffset(vec2 offset);
        void addSizeScale(vec2 scale);
        void addRotationOffset(float offset);

};

#include "pancake/core/entity.hpp"