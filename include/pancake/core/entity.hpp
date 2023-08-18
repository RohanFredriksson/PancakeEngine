#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

using std::string;
using std::vector;
using glm::vec2;
using json = nlohmann::json;

namespace Pancake {

    class Component;

    class Entity {

        private:

            int id;
            bool started;
            vector<Component*> components;
            vec2 position;
            vec2 size;
            float rotation;
            bool serialisable;
            bool dead;

            void init(int id, vec2 position, vec2 size, float radians, bool load);

        public:

            Entity(float x, float y, float w, float h, float r);
            Entity(float x, float y, float w, float h);
            Entity(float x, float y);
            Entity(vec2 position, vec2 size, float radians);
            Entity(vec2 position, vec2 size);
            Entity(vec2 position);
            Entity();
            ~Entity();
            
            void start();
            void update(float dt);
            json serialise();
            static Entity* load(json j);
            void imgui();
            void kill();

            // Getter Methods.
            int getId();
            vector<Component*> getComponents();
            vec2 getPosition();
            vec2 getSize();
            float getRotation();
            bool isSerialisable();
            bool isDead();

            // Setter Methods.
            void setId(int id);
            void setPosition(vec2 position);
            void setSize(vec2 size);
            void setRotation(float radians);
            void setSerialisable(bool serialisable);

            // Adder Methods.
            void addPosition(vec2 position);
            void addSize(vec2 size);
            void addRotation(float radians);
            void addRotationAround(float radians, vec2 around);

            // Component Methods
            Component* getComponent(string type);
            void addComponent(Component* component);

    };

}

#include "pancake/core/component.hpp"