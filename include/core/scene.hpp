#pragma once

#include <string>
#include <map>
#include <core/camera.hpp>
#include <core/entity.hpp>

using std::string;
using std::map;

class Scene {

    private:

        string name;
        map<int, Entity*> entities;
        Camera* camera;
        //Renderer renderer;
        //World physics;

        void addNewComponents();
        
    public:

        Scene(string name);
        ~Scene();

        void update(float dt);
        void render();

};