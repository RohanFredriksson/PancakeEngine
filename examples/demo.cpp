// A physics sandbox showing the main parts of the engine.
//
//   Left click        spawn a box (or select an entity while the inspector is open)
//   Shift left click  spawn a ball
//   Right drag        pan the camera
//   Scroll            zoom
//   D                 toggle physics debug drawing
//   I                 toggle the inspector (drag to move, Delete, Ctrl+D to duplicate)
//   `                 toggle the console (try "help")
//   P                 pause or resume
//   N                 step one physics frame (pauses)
//   F                 toggle the frame rate overlay
//   R                 reset the sandbox
//   Gamepad           left stick pans, A drops a box
//   Escape            quit
//
// Run with --frames N to quit automatically after N frames.

#include <cstdlib>
#include <cstring>
#include <string>
#include <imgui.h>
#include "pancake/pancake.hpp"

using namespace Pancake;

namespace {

    int frameLimit = -1;
    int frames = 0;

    vec4 randomColour() {
        float r = 0.3f + 0.7f * (std::rand() / (float) RAND_MAX);
        float g = 0.3f + 0.7f * (std::rand() / (float) RAND_MAX);
        float b = 0.3f + 0.7f * (std::rand() / (float) RAND_MAX);
        return vec4(r, g, b, 1.0f);
    }

}

// Flashes the entity's sprite white whenever it hits something hard.
class ImpactFlash : public Component, public CollisionListener {

    private:

        float flash;
        float speed; // Last frame's speed. Collision events arrive after the step has resolved the impact.

    public:

        ImpactFlash() : Component("ImpactFlash") {
            this->flash = 0.0f;
            this->speed = 0.0f;
            this->setImguiable(false);
        }

        void collisionEnter(Entity* with, CollisionManifold manifold) override {
            if (this->speed > 6.0f) {this->flash = 0.15f;}
            if (this->speed > 12.0f) {Pancake::getScene()->getCamera()->shake(0.15f, 0.25f);}
        }

        void update(float dt) override {
            Rigidbody* r = this->getEntity()->getComponent<Rigidbody>();
            if (r != nullptr) {this->speed = glm::length(r->getVelocity());}
            SpriteRenderer* sprite = this->getEntity()->getComponent<SpriteRenderer>();
            if (sprite == nullptr) {return;}
            if (this->flash > 0.0f) {
                this->flash -= dt;
                sprite->setColour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
                if (this->flash <= 0.0f) {sprite->setColour(this->colour);}
            }
        }

        vec4 colour = vec4(1.0f);

};

REGISTER(Component, ImpactFlash);

// Sprites are rectangles, so balls draw their outline with debug lines.
class BallOutline : public Component {

    public:

        BallOutline() : Component("BallOutline") {
            this->setImguiable(false);
        }

        void update(float dt) override {
            Rigidbody* r = this->getEntity()->getComponent<Rigidbody>();
            if (r == nullptr) {return;}
            for (Collider* c : r->getColliders()) {
                CircleCollider* circle = dynamic_cast<CircleCollider*>(c);
                if (circle == nullptr) {continue;}
                vec2 centre = circle->getPosition();
                float rotation = this->getEntity()->getRotation();
                DebugDraw::drawCircleCollider(centre, circle->getRadius(), vec3(1.0f, 0.8f, 0.3f), 1);
                DebugDraw::drawLine(centre, centre + circle->getRadius() * vec2(cosf(rotation), sinf(rotation)), vec3(1.0f, 0.8f, 0.3f), 1);
            }
        }

};

REGISTER(Component, BallOutline);

void spawnBox(Scene* scene, vec2 position, vec2 size, float mass, vec4 colour) {
    Entity* e = new Entity(position, size, 0.0f);

    SpriteRenderer* sprite = new SpriteRenderer();
    sprite->setColour(colour);
    e->addComponent(sprite);

    Rigidbody* rigidbody = new Rigidbody();
    BoxCollider* box = new BoxCollider();
    box->setSize(size);
    box->setMass(mass);
    rigidbody->addCollider(box);
    rigidbody->setFriction(0.4f);
    rigidbody->setRestitution(0.2f);
    e->addComponent(rigidbody);

    if (mass > 0.0f) {
        ImpactFlash* flash = new ImpactFlash();
        flash->colour = colour;
        e->addComponent(flash);

        // Pop in: the sprite grows to full size while the collider is full size from the start.
        e->setSize(0.2f * size);
        Tween::size(e, size, 0.25f, Ease::outBack);
    }

    scene->addEntity(e);
}

void spawnBall(Scene* scene, vec2 position, float radius) {
    Entity* e = new Entity(position, vec2(2.0f * radius), 0.0f);
    Rigidbody* rigidbody = new Rigidbody();
    CircleCollider* circle = new CircleCollider();
    circle->setRadius(radius);
    circle->setMass(1.0f);
    rigidbody->addCollider(circle);
    rigidbody->setFriction(0.3f);
    rigidbody->setRestitution(0.5f);
    e->addComponent(rigidbody);
    e->addComponent(new BallOutline());
    scene->addEntity(e);
}

// Handles input for the sandbox. It is not serialised, so saved scenes only contain the bodies.
class Sandbox : public Component {

    public:

        Sandbox() : Component("Sandbox") {
            this->setSerialisable(false);
            this->setImguiable(false);
        }

        void update(float dt) override {

            frames++;
            if (frameLimit > 0 && frames >= frameLimit) {Pancake::stop();}

            // Keyboard shortcuts are ignored while typing into an ImGui window, such as the console.
            if (!ImGui::GetIO().WantCaptureKeyboard) {
                if (KeyListener::isKeyBeginDown(GLFW_KEY_ESCAPE)) {Pancake::stop();}
                bool control = KeyListener::isKeyDown(GLFW_KEY_LEFT_CONTROL) || KeyListener::isKeyDown(GLFW_KEY_RIGHT_CONTROL);
                if (KeyListener::isKeyBeginDown(GLFW_KEY_D) && !control) {Console::execute("debug");} // Ctrl+D duplicates in the inspector.
                if (KeyListener::isKeyBeginDown(GLFW_KEY_I)) {Pancake::inspector();}
                if (KeyListener::isKeyBeginDown(GLFW_KEY_R)) {Pancake::reset(); Pancake::load(build);}
                if (KeyListener::isKeyBeginDown(GLFW_KEY_P)) {Pancake::pause();}
                if (KeyListener::isKeyBeginDown(GLFW_KEY_N)) {Pancake::step();}
                if (KeyListener::isKeyBeginDown(GLFW_KEY_F)) {Console::execute("fps");}
            }
            if (KeyListener::isKeyBeginDown(GLFW_KEY_GRAVE_ACCENT)) {Pancake::console();}

            // Gamepad: the left stick pans, even while paused, and A drops a box at the centre of the view.
            Camera* view = Pancake::getScene()->getCamera();
            view->addPosition(GamepadListener::getLeftStick() * 10.0f * Pancake::getUnscaledDeltaTime());
            if (GamepadListener::isButtonBeginDown(GLFW_GAMEPAD_BUTTON_A)) {
                spawnBox(Pancake::getScene(), view->getPosition(), vec2(0.8f, 0.8f), 1.0f, randomColour());
            }

            if (ImGui::GetIO().WantCaptureMouse) {return;}
            Camera* camera = Pancake::getScene()->getCamera();
            vec2 mouse = vec2(MouseListener::getWorldX(), MouseListener::getWorldY());

            // Spawn bodies, unless the click is selecting an entity in the inspector.
            if (MouseListener::isMouseBeginDown(GLFW_MOUSE_BUTTON_LEFT) && !Inspector::opened()) {
                bool shift = KeyListener::isKeyDown(GLFW_KEY_LEFT_SHIFT) || KeyListener::isKeyDown(GLFW_KEY_RIGHT_SHIFT);
                if (shift) {spawnBall(Pancake::getScene(), mouse, 0.4f);}
                else {spawnBox(Pancake::getScene(), mouse, vec2(0.8f, 0.8f), 1.0f, randomColour());}
            }

            // Pan with the right mouse button and zoom with the scroll wheel.
            if (MouseListener::isMouseDown(GLFW_MOUSE_BUTTON_RIGHT) && MouseListener::isMouseDragging()) {
                camera->addPosition(vec2(-MouseListener::getWorldDx(), -MouseListener::getWorldDy()));
            }
            if (MouseListener::getScrollY() != 0.0) {
                camera->scaleZoom(MouseListener::getScrollY() > 0.0 ? 1.1f : 1.0f / 1.1f);
            }

        }

        static void build(Scene* scene) {

            scene->setName("Sandbox");
            scene->getPhysics()->setGravity(vec2(0.0f, -10.0f));
            scene->getCamera()->setPosition(vec2(0.0f, 2.0f));
            scene->getCamera()->setProjectionHeight(17.0f);

            Entity* input = new Entity();
            input->addComponent(new Sandbox());
            scene->addEntity(input);

            // Instructions.
            Entity* text = new Entity(vec2(0.0f, 9.6f), vec2(0.45f, 0.45f), 0.0f);
            TextRenderer* label = new TextRenderer();
            label->setText("Click: box   Shift+Click: ball   D: debug   I: inspector   `: console   R: reset");
            label->setAlignment(TextRenderer::CENTER);
            text->addComponent(label);
            scene->addEntity(text);

            // Ground and walls.
            vec4 grey = vec4(0.35f, 0.35f, 0.4f, 1.0f);
            spawnBox(scene, vec2(0.0f, -4.5f), vec2(20.0f, 1.0f), 0.0f, grey);
            spawnBox(scene, vec2(-10.5f, 1.0f), vec2(1.0f, 12.0f), 0.0f, grey);
            spawnBox(scene, vec2(10.5f, 1.0f), vec2(1.0f, 12.0f), 0.0f, grey);

            // A pyramid of crates and a few balls.
            for (int row = 0; row < 5; row++) {
                for (int i = 0; i <= 4 - row; i++) {
                    float x = -2.0f + i * 1.0f + row * 0.5f;
                    spawnBox(scene, vec2(x, -3.5f + row * 1.0f), vec2(1.0f, 1.0f), 1.0f, randomColour());
                }
            }
            for (int i = 0; i < 3; i++) {spawnBall(scene, vec2(-6.0f + i * 1.2f, 3.0f + i), 0.5f);}

        }

};

int main(int argc, char* argv[]) {

    for (int i = 1; i < argc - 1; i++) {
        if (std::strcmp(argv[i], "--frames") == 0) {frameLimit = std::atoi(argv[i + 1]);}
    }

    Pancake::title("Pancake Sandbox");
    Pancake::size(1024, 768);
    Pancake::load(Sandbox::build);
    Pancake::start();
    return 0;

}
