#pragma once

#include <string>
#include <unordered_map>
#include <core/camera.hpp>
#include <core/entity.hpp>
#include <core/component.hpp>
#include <graphics/renderer/renderer.hpp>
#include <physics/world.hpp>

using std::string;
using std::unordered_map;

class Scene {

    private:

        string name;
        unordered_map<int, Entity*> entities;
        unordered_map<int, Component*> components;
        Camera* camera;
        Renderer* renderer;
        World* physics;

        void addNewComponents();
        
    public:

        Scene(string name);
        ~Scene();

        void update(float dt);
        void render();

        Camera* getCamera();
        Renderer* getRenderer();
        World* getPhysics();

        void addEntity(Entity* entity);

};