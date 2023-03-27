#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

using std::vector;
using std::string;
using glm::vec2;
using json = nlohmann::json;

class Entity;

class Component {

    private:

        int id;
        string type;
        Entity* entity;
        bool dead;

        void (*keyDown)(Component* component);
        void (*mouseDown)(Component* component);
        //void (*mouseDownOnComponent)(Component* component);
        void (*mouseDragging)(Component* component);
        bool callbackUpdate;

    public:

        Component(string type);
        virtual ~Component();
        virtual void update(float dt);
        virtual json serialise();
        virtual void onCollision(Component* with);
        void kill();
        
        int getId();
        Entity* getEntity();
        bool isDead();
        
        void keyDownCallback();
        //void mouseDownOnComponentCallback();
        void mouseDownCallback();
        void mouseDraggingCallback();

        bool hasKeyDownCallback();
        //bool hasMouseDownOnComponentCallback();
        bool hasMouseDownCallback();
        bool hasMouseDraggingCallback();
        bool isCallbackUpdated();
        
        void setId(int id);
        void setEntity(Entity* entity);
    
        void setKeyDownCallback(void (*callback)(Component* component));
        //void setMouseDownOnComponentCallback(void (*callback)(Component* component));
        void setMouseDownCallback(void (*callback)(Component* component));
        void setMouseDraggingCallback(void (*callback)(Component* component));
        void clearCallbackUpdate();

};

class TransformableComponent : public Component {

    private:

        vec2 positionOffset;
        vec2 sizeScale;
        float rotationOffset;

    public:

        TransformableComponent(string type);
        virtual json serialise();

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