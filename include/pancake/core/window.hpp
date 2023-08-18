#pragma once

#include "pancake/core/console.hpp"
#include "pancake/core/scene.hpp"

namespace Pancake {

    namespace Window {

        void load(void(*method)(Scene* scene));
        void load(string filename);
        void save(string filename);
        void reset();

        void start();
        void stop();

        int getWidth();
        int getHeight();
        float getAspectRatio();
        Scene* getScene();
        
        void setIcon(string filename);
        void setTitle(string title);
        void setWidth(int w);
        void setHeight(int h);
        void resetFramebuffers();
        int readPixel(int x, int y);

        void openConsole();
        void closeConsole();

    }

}