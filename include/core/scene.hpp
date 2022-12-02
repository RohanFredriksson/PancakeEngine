#include <string>
#include <vector>

#pragma once

using std::string;
using std::vector;

namespace scene {

    class Scene {

        private:

            string name;
            //vector<Entity> entities;
            //Camera camera;
            //Renderer renderer;
            //World physics;

        public:

            Scene(string name) {
                this->name = name;
            }

            void update(float dt);
            void render();

    };

}