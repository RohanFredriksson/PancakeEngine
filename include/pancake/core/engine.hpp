#pragma once

#include "pancake/core/scene.hpp"
#include "pancake/core/entity.hpp"
#include "pancake/core/component.hpp"

namespace Pancake {
    
    void load(void (*method)(Scene* scene));
    void load(string filename);
    void save(string filename);
    void reset();

    void start();
    void stop();

    void icon(string filename);
    void title(string title);
    
    Scene* getScene();
    Entity* getEntity(int id);
    Component* getComponent(int id);

}