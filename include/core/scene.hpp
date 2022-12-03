#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

class Scene {

    private:

        string name;
        //vector<Entity> entities;
        //Camera camera;
        //Renderer renderer;
        //World physics;

    public:

        Scene(std::string name);
        ~Scene();

        void update(float dt);
        void render();

};