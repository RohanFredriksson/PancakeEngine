#pragma once

#include <functional>

#include "pancake/core/scene.hpp"
#include "pancake/core/entity.hpp"
#include "pancake/core/component.hpp"

namespace Pancake {
    
    void load(std::function<void(Scene*)> method);
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

    void gravity(vec2 gravity);
    void debug(bool state);

    void console(bool state);
    void console();

    void inspector(bool state);
    void inspector();
    

    // Time. Pausing keeps components updating, with a delta time of zero, so physics, timers and tweens stop.
    void timeScale(float scale);
    float getTimeScale();
    void pause(bool state);
    void pause();
    bool isPaused();
    void step(int frames = 1); // Pause if needed, then advance the given number of physics steps, one per frame.
    float getFps();
    float getUnscaledDeltaTime();

    void vsync(bool state);
    void fullscreen(bool state);
    void fullscreen();
    bool isFullscreen();

    Scene* getScene();
    Entity* getEntity(int id);
    Component* getComponent(int id);

}