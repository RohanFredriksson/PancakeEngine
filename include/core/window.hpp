#pragma once

#include "core/scene.hpp"
#include "soloud/include/soloud.h"

using SoLoud::Soloud;

namespace Window {
    
    void run();
    
    int getWidth();
    int getHeight();
    float getAspectRatio();
    Scene* getScene();
    Soloud* getAudioEngine();

    void setWidth(int w);
    void setHeight(int h);
    void resetFramebuffers();
    int readPixel(int x, int y);

}