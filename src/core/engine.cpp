#include "pancake/pancake.hpp"
#include "pancake/core/engine.hpp"
#include "pancake/audio/audioengine.hpp"
#include "pancake/core/window.hpp"

#include "pancake/core/factory.hpp"
#include "pancake/core/component.hpp"
#include "pancake/physics/collider.hpp"

namespace Pancake {

    void load(void (*method)(Scene* scene)) {
        Window::load(method);
    }

    void load(string filename) {
        Window::load(filename);
    }

    void save(string filename) {
        Window::save(filename);
    }

    void reset() {
        Window::reset();
    }

    void start() {
        AudioEngine::init();
        Window::start();
        AudioEngine::destroy();
    }

    void stop() {
        Window::stop();
    }

    void icon(string filename) {
        Window::setIcon(filename);
    }

    void title(string title) {
        Window::setTitle(title);
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