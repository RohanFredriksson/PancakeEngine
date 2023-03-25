#include "pancake/core/engine.hpp"
#include "pancake/core/factory.hpp"
#include "pancake/audio/audioengine.hpp"
#include "pancake/window/window.hpp"

#include "pancake/audio/audioplayer.hpp"
#include "pancake/graphics/renderer/spriterenderer.hpp"
#include "pancake/graphics/renderer/textrenderer.hpp"
#include "pancake/physics/primitives/rigidbody.hpp"

namespace {

    void registry() {
        Factory::add("AudioPlayer", AudioPlayer::create);
        Factory::add("SpriteRenderer", SpriteRenderer::create);
        Factory::add("TextRenderer", TextRenderer::create);
        Factory::add("Rigidbody", Rigidbody::create);
    }

}

namespace Engine {

    int run(string name, string filename, void(*init)(Scene* scene)) {
        registry();
        AudioEngine::init();
        if (!Window::init(name, filename, init)) {return 1;}
        Window::loop();
        Window::destroy();
        AudioEngine::destroy();
        return 0;
    }

    int run(string name, void (*init)(Scene* scene)) {
        registry();
        AudioEngine::init();
        if (!Window::init(name, init)) {return 1;}
        Window::loop();
        Window::destroy();
        AudioEngine::destroy();
        return 0;
    }

    int run(string name, string filename) {
        registry();
        AudioEngine::init();
        if (!Window::init(name, filename)) {return 1;}
        Window::loop();
        Window::destroy();
        AudioEngine::destroy();
        return 0;
    }

}