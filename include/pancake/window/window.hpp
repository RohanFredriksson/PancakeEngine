#pragma once

#include "pancake/core/scene.hpp"

namespace Window {
    
    bool init(string name, string filename, void(*init)(Scene* scene));
    void load(string name, string filename, void(*init)(Scene* scene));
    void save(string filename);
    void loop();
    void destroy();

    int getWidth();
    int getHeight();
    float getAspectRatio();
    Scene* getScene();

    void setWidth(int w);
    void setHeight(int h);
    void resetFramebuffers();
    int readPixel(int x, int y);

}