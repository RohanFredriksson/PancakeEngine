#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <imgui.h>

#include "pancake/core/factory.hpp"

using std::vector;
using std::string;
using glm::vec2;
using json = nlohmann::json;

namespace Pancake {

    class Entity;

    class Component {

        protected:

            int id;
            string type;
            Entity* entity;
            bool serialisable;
            bool imguiable;
            bool dead;

            void init(int id, string type, bool load);

        public:

            Component(string type);
            virtual ~Component();
            virtual void start();
            virtual void end();
            virtual void update(float dt);
            virtual json serialise();
            virtual bool load(json j);
            virtual void imgui();
            void kill();
            
            int getId();
            string getType();
            Entity* getEntity();
            bool isSerialisable();
            bool isImguiable();
            bool isDead();
            
            void setId(int id);
            void setEntity(Entity* entity);
            void setSerialisable(bool serialisable);
            void setImguiable(bool imguiable);

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
            virtual void imgui() override;

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

}

#include "pancake/core/entity.hpp"