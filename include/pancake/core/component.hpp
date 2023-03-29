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

    protected:

        int id;
        string type;
        Entity* entity;
        bool dead;

        void init(int id, string type);

    public:

        Component(string type);
        virtual ~Component();
        virtual void update(float dt);
        virtual json serialise();
        virtual bool load(json j);
        virtual void onCollision(Component* with);
        void kill();
        
        int getId();
        Entity* getEntity();
        bool isDead();
        
        void setId(int id);
        void setEntity(Entity* entity);

};

class TransformableComponent : public Component {

    protected:

        vec2 positionOffset;
        vec2 sizeScale;
        float rotationOffset;

    public:

        TransformableComponent(string type);
        virtual json serialise() override;
        virtual bool load(json j) override;

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

class KeyDown {
    public: 
        virtual void keyDownCallback();
};

class MouseDown {
    public: 
        virtual void mouseDownCallback();
};

class MouseDragging {
    public: 
        virtual void mouseDraggingCallback();
};

#include "pancake/core/entity.hpp"