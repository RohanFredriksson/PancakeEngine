#pragma once

#include "pancake/core/scene.hpp"

namespace Pancake {
    
    int run(string name, string filename, void(*init)(Scene* scene));
    int run(string name, void (*init)(Scene* scene));
    int run(string name, string filename);

    void load(string name, string filename, void(*init)(Scene* scene));
    void load(string name, void (*init)(Scene* scene));
    void load(string name, string filename);

    void save(string filename);

    Scene* getScene();

}