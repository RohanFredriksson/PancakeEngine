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

        void (*keyPress)(Component* component, int key);
        void (*keyBeginPress)(Component* component, int key);
        void (*mouseDown)(Component* component, int button);
        void (*mouseBeginDown)(Component* component, int button);
        void (*mouseDownOnComponent)(Component* component, int button);
        void (*mouseBeginDownOnComponent)(Component* component, int button);
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
        
        void keyPressCallback(int key);
        void keyBeginPressCallback(int key);
        void mouseDownOnComponentCallback(int button);
        void mouseBeginDownOnComponentCallback(int button);
        void mouseDownCallback(int button);
        void mouseBeginDownCallback(int button);
        void mouseDraggingCallback();

        bool hasKeyPressCallback();
        bool hasKeyBeginPressCallback();
        bool hasMouseDownOnComponentCallback();
        bool hasMouseBeginDownOnComponentCallback();
        bool hasMouseDownCallback();
        bool hasMouseBeginDownCallback();
        bool hasMouseDraggingCallback();
        bool isCallbackUpdated();
        
        void setId(int id);
        void setEntity(Entity* entity);
    
        void setKeyPressCallback(void (*callback)(Component* component, int key));
        void setKeyBeginPressCallback(void (*callback)(Component* component, int key));
        void setMouseDownOnComponentCallback(void (*callback)(Component* component, int button));
        void setMouseBeginDownOnComponentCallback(void (*callback)(Component* component, int button));
        void setMouseDownCallback(void (*callback)(Component* component, int button));
        void setMouseBeginDownCallback(void (*callback)(Component* component, int button));
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