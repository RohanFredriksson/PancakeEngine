#pragma once

#include "core/scene.hpp"

namespace Window {
    
    bool init();
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