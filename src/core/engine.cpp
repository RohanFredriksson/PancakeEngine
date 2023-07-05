#include "pancake/pancake.hpp"
#include "pancake/core/engine.hpp"
#include "pancake/audio/audioengine.hpp"
#include "pancake/core/window.hpp"

#include "pancake/core/component.hpp"
#include "pancake/audio/audioplayer.hpp"
#include "pancake/graphics/spriterenderer.hpp"
#include "pancake/graphics/textrenderer.hpp"
#include "pancake/physics/rigidbody.hpp"
#include "pancake/components/animation.hpp"
#include "pancake/components/transition.hpp"

#include "pancake/physics/collider.hpp"
#include "pancake/pancake.hpp"

#include "pancake/core/factory.hpp"

namespace Pancake {

    namespace {

        // Methods to create polymorphic components.
        void* AudioPlayerCreate()    {return new AudioPlayer();}
        void* SpriteRendererCreate() {return new SpriteRenderer();}
        void* TextRendererCreate()   {return new TextRenderer();}
        void* RigidbodyCreate()      {return new Rigidbody();}
        void* AnimationCreate()      {return new Animation();}
        void* FadeTransitionCreate() {return new FadeTransition();}

        // Methods to create polymorphic colliders.
        void* BoxCreate()    {return new Box();}
        void* CircleCreate() {return new Circle();}

        void registry() {
            
            // Add these methods to their corresponding factories, so we can load game state from files.
            ComponentFactory::add("AudioPlayer", AudioPlayerCreate);
            ComponentFactory::add("SpriteRenderer", SpriteRendererCreate);
            ComponentFactory::add("TextRenderer", TextRendererCreate);
            ComponentFactory::add("Rigidbody", RigidbodyCreate);
            ComponentFactory::add("Animation", AnimationCreate);
            ComponentFactory::add("FadeTransition", FadeTransitionCreate);

            ColliderFactory::add("Box", BoxCreate);
            ColliderFactory::add("Circle", CircleCreate);
        
        }

    }

    int run(string name, string filename, void(*init)(Scene* scene)) {

        FACTORY(Pancake::Component).list();

        registry();
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