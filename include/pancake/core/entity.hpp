#pragma once

#include <string>
#include <vector>
#include <utility>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace Pancake {

    class Component;

    class Entity {

        private:

            int id;
            std::string name;
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
            std::string getName();
            std::vector<Component*> getComponents();
            glm::vec2 getPosition();
            glm::vec2 getSize();
            float getRotation();
            bool isSerialisable();
            bool isDead();

            // Setter Methods.
            void setId(int id);
            void setName(std::string name);
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

            // Typed component access. These match subclasses too, e.g. getComponent<ForceGenerator>().
            template<class T> T* getComponent();
            template<class T> std::vector<T*> getComponents();
            template<class T> bool hasComponent();
            template<class T, class... Args> T* addComponent(Args&&... args);

    };

}

#include "pancake/core/component.hpp"

namespace Pancake {

    template<class T>
    T* Entity::getComponent() {
        for (Component* c : this->components) {
            T* t = dynamic_cast<T*>(c);
            if (t != nullptr && !c->isDead()) {return t;}
        }
        return nullptr;
    }

    template<class T>
    std::vector<T*> Entity::getComponents() {
        std::vector<T*> result;
        for (Component* c : this->components) {
            T* t = dynamic_cast<T*>(c);
            if (t != nullptr && !c->isDead()) {result.push_back(t);}
        }
        return result;
    }

    template<class T>
    bool Entity::hasComponent() {
        return this->getComponent<T>() != nullptr;
    }

    template<class T, class... Args>
    T* Entity::addComponent(Args&&... args) {
        T* component = new T(std::forward<Args>(args)...);
        this->addComponent(component);
        return component;
    }

}
