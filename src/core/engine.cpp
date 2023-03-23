#include "pancake/core/engine.hpp"
#include "pancake/audio/audioengine.hpp"
#include "pancake/window/window.hpp"

namespace Engine {

    int run(string name, string filename, void(*init)(Scene* scene)) {
        AudioEngine::init();
        if (!Window::init(name, filename, init)) {return 1;}
        Window::loop();
        Window::destroy();
        AudioEngine::destroy();
        return 0;
    }

    int run(string name, void (*init)(Scene* scene)) {
        AudioEngine::init();
        if (!Window::init(name, init)) {return 1;}
        Window::loop();
        Window::destroy();
        AudioEngine::destroy();
        return 0;
    }

    int run(string name, string filename) {
        AudioEngine::init();
        if (!Window::init(name, filename)) {return 1;}
        Window::loop();
        Window::destroy();
        AudioEngine::destroy();
        return 0;
    }

}