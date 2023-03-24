#pragma once

#include <string>
#include <unordered_map>
#include "pancake/core/camera.hpp"
#include "pancake/core/entity.hpp"
#include "pancake/core/component.hpp"
#include "pancake/graphics/renderer/renderer.hpp"
#include "pancake/physics/world.hpp"

using std::string;
using std::unordered_map;

class Scene {

    private:

        string name;
        unordered_map<int, Entity*> entities;
        unordered_map<int, Component*> components;

        unordered_map<int, Component*> keyPressComponents;
        unordered_map<int, Component*> keyBeginPressComponents;
        unordered_map<int, Component*> mouseButtonDownComponents;
        unordered_map<int, Component*> mouseButtonBeginDownComponents;
        unordered_map<int, Component*> mouseDraggingComponents;
        
        Camera* camera;
        Renderer* renderer;
        World* physics;

        void addNewComponents();
        void removeDeadComponents();
        void updateCallbackComponents();

    public:

        Scene(string name, void (*init)(Scene* scene));
        Scene(string name, string filename);
        Scene(string name, string filename, void (*init)(Scene* scene));
        ~Scene();

        void update(float dt);
        void render();

        Camera* getCamera();
        Renderer* getRenderer();
        World* getPhysics();

        void addEntity(Entity* entity);
        Entity* getEntityById(int id);
        Component* getComponentById(int id);

};