#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace Pancake {

    class Component;

    class Entity {

        private:

            int id;
            bool started;
            std::vector<Component*> components;
            glm::vec2 position;
            glm::vec2 size;
            float rotation;
            bool serialisable;
            bool dead;

            void init(int id, glm::vec2 position, glm::vec2 size, float radians, bool load);

        public:

            Entity(float x, float y, float w, float h, float r);
            Entity(float x, float y, float w, float h);
            Entity(float x, float y);
            Entity(glm::vec2 position, glm::vec2 size, float radians);
            Entity(glm::vec2 position, glm::vec2 size);
            Entity(glm::vec2 position);
            Entity();
            ~Entity();
            
            void start();
            void update(float dt);
            nlohmann::json serialise();
            static Entity* load(nlohmann::json j);
            void imgui();
            void kill();

            // Getter Methods.
            int getId();
            std::vector<Component*> getComponents();
            glm::vec2 getPosition();
            glm::vec2 getSize();
            float getRotation();
            bool isSerialisable();
            bool isDead();

            // Setter Methods.
            void setId(int id);
            void setPosition(glm::vec2 position);
            void setSize(glm::vec2 size);
            void setRotation(float radians);
            void setSerialisable(bool serialisable);

            // Adder Methods.
            void addPosition(glm::vec2 position);
            void addSize(glm::vec2 size);
            void addRotation(float radians);
            void addRotationAround(float radians, glm::vec2 around);

            // Component Methods
            Component* getComponent(std::string type);
            void addComponent(Component* component);

    };

}

#include "pancake/core/component.hpp"