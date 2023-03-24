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

        void (*keyPress)(Component* component);
        void (*mouseButtonDown)(Component* component);
        //void (*mouseButtonDownOnComponent)(Component* component);
        void (*mouseDragging)(Component* component);
        bool callbackUpdate;

    public:

        Component();
        virtual ~Component();
        virtual void update(float dt);
        virtual void onCollision(Component* with);
        void kill();
        
        int getId();
        Entity* getEntity();
        bool isDead();
        
        void keyPressCallback();
        //void mouseButtonDownOnComponentCallback();
        void mouseButtonDownCallback();
        void mouseDraggingCallback();

        bool hasKeyPressCallback();
        //bool hasMouseButtonDownOnComponentCallback();
        bool hasMouseButtonDownCallback();
        bool hasMouseDraggingCallback();
        bool isCallbackUpdated();
        
        void setId(int id);
        void setEntity(Entity* entity);
    
        void setKeyPressCallback(void (*callback)(Component* component));
        //void setMouseButtonDownOnComponentCallback(void (*callback)(Component* component));
        void setMouseButtonDownCallback(void (*callback)(Component* component));
        void setMouseDraggingCallback(void (*callback)(Component* component));
        void clearCallbackUpdate();

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