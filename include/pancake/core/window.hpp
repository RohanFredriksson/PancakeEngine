#pragma once

#include <functional>

#include "pancake/core/console.hpp"
#include "pancake/core/scene.hpp"

namespace Pancake {

    namespace Window {

        void load(std::function<void(Scene*)> method);
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
        void setProjectionSize(vec2 size);
        void setProjectionSize(float height);
        void setProjectionHeight(float height);
        int readPixel(int x, int y);

        // Time. The scene is updated with the frame time multiplied by the time scale, or zero while paused.
        void setTimeScale(float scale);
        float getTimeScale();
        void setPaused(bool paused);
        bool isPaused();
        void step(int frames);
        float getFps();
        float getUnscaledDeltaTime();

        // Display.
        void setVsync(bool vsync);
        bool hasVsync();
        void setFullscreen(bool fullscreen);
        bool isFullscreen();

        void openConsole();
        void closeConsole();

    }

}