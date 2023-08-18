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

    void size(int w, int h);
    void width(int w);
    void height(int h);

    void projection(vec2 size);
    void projection(float height);

    void console(bool state);
    void console();
    
    Scene* getScene();
    Entity* getEntity(int id);
    Component* getComponent(int id);

}