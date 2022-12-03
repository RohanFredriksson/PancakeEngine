#pragma once

#include <string>
#include <vector>
#include <core/camera.hpp>
#include <core/entity.hpp>

using std::string;
using std::vector;

class Scene {

    private:

        string name;
        vector<Entity*> entities;
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