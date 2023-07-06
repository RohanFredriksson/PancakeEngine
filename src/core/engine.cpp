#include "pancake/pancake.hpp"
#include "pancake/core/engine.hpp"
#include "pancake/audio/audioengine.hpp"
#include "pancake/core/window.hpp"

#include "pancake/core/factory.hpp"
#include "pancake/core/component.hpp"
#include "pancake/physics/collider.hpp"

namespace Pancake {

    int run(string name, string filename, void(*init)(Scene* scene)) {
        AudioEngine::init();
        if (!Window::init(name, filename, init)) {return 1;}
        Window::loop();
        Window::destroy();
        AudioEngine::destroy();
        return 0;
    }

    int run(string name, void (*init)(Scene* scene)) {
        return run(name, "", init);
    }

    int run(string name, string filename) {
        return run(name, filename, nullptr);
    }

    void load(string name, string filename, void(*init)(Scene* scene)) {
        Window::load(name, filename, init);
    }

    void load(string name, void (*init)(Scene* scene)) {
        Window::load(name, "", init);
    }

    void load(string name, string filename) {
        Window::load(name, filename, nullptr);
    }

    void save(string filename) {
        Window::save(filename);
    }

    Scene* getScene() {
        return Window::getScene();
    }

    Entity* getEntity(int id) {
        return Window::getScene()->getEntity(id);
    }

    Component* getComponent(int id) {
        return Window::getScene()->getComponent(id);
    }

}