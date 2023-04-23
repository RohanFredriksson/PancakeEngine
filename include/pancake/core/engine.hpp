#pragma once

#include "pancake/core/scene.hpp"
#include "pancake/core/entity.hpp"
#include "pancake/core/component.hpp"

namespace Pancake {
    
    int run(string name, string filename, void(*init)(Scene* scene));
    int run(string name, void (*init)(Scene* scene));
    int run(string name, string filename);

    void load(string name, string filename, void(*init)(Scene* scene));
    void load(string name, void (*init)(Scene* scene));
    void load(string name, string filename);

    void save(string filename);

    Scene* getScene();
    Entity* getEntity(int id);
    Component* getComponent(int id);

}