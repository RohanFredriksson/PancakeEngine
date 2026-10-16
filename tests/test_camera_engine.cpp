// Runs the real engine loop to check CameraFollow finding its target through the scene, camera shake from
// the console, and gamepads being polled every frame.

#include <cstdio>
#include "test.hpp"
#include "pancake/pancake.hpp"

using namespace Pancake;

namespace {

    int frame = 0;
    int shakeStarted = -1;
    int faded = -1;

    void log(const char* message) {
        std::printf("frame %d: %s\n", frame, message);
        std::fflush(stdout);
    }

    Camera* camera() {
        return Pancake::getScene()->getCamera();
    }

}

class Driver : public Component {

    public:

        Driver() : Component("Driver") {}

        void update(float dt) override {

            frame++;
            Scene* scene = Pancake::getScene();
            Entity* player = scene->findEntity("Player");
            Entity* other = scene->findEntity("Other");

            if (frame == 3) {
                log("the camera starts on the player");
                CHECK(camera()->getPosition() == vec2(5.0f, 5.0f));
                player->setPosition(vec2(8.0f, 5.0f));
            }

            if (frame == 5) {
                log("it follows the player, then another target");
                CHECK(camera()->getPosition() == vec2(8.0f, 5.0f));
                player->getComponent<CameraFollow>()->setTarget(other);
            }

            if (frame == 7) {
                CHECK(camera()->getPosition() == vec2(-5.0f, 0.0f));
                other->kill(); // The camera stays put once its target is gone.
            }

            if (frame == 9) {
                CHECK(camera()->getPosition() == vec2(-5.0f, 0.0f));
                log("shake from the console");
                Console::execute("shake 1 0.2");
                CHECK(camera()->isShaking());
                shakeStarted = frame;
            }

            if (shakeStarted > 0 && frame > shakeStarted && !camera()->isShaking()) {
                log("the shake has faded");
                CHECK(camera()->getShakeOffset() == vec2(0.0f));
                CHECK(camera()->getPosition() == vec2(-5.0f, 0.0f));
                shakeStarted = -1;
                faded = frame;

                // A fake pad is replaced by what the next poll finds, which is nothing.
                GLFWgamepadstate state = {};
                state.buttons[GLFW_GAMEPAD_BUTTON_A] = GLFW_PRESS;
                GamepadListener::setState(0, true, state, "Fake");
                CHECK(GamepadListener::isButtonDown(GLFW_GAMEPAD_BUTTON_A));
            }

            if (shakeStarted > 0 && frame - shakeStarted > 5000) {log("the shake never faded"); CHECK(false); Pancake::stop();}

            if (faded > 0 && frame == faded + 1) {
                log("gamepads are polled every frame");
                CHECK(!GamepadListener::isConnected(0));
                CHECK(!GamepadListener::isButtonDown(GLFW_GAMEPAD_BUTTON_A));
                CHECK(GamepadListener::isButtonEndDown(GLFW_GAMEPAD_BUTTON_A));
                Console::execute("gamepads");
            }

            if (faded > 0 && frame == faded + 3) {Pancake::stop();}

        }

};

void init(Scene* scene) {
    Entity* player = new Entity(vec2(5.0f, 5.0f));
    player->setName("Player");
    player->addComponent<CameraFollow>();
    scene->addEntity(player);

    Entity* other = new Entity(vec2(-5.0f, 0.0f));
    other->setName("Other");
    scene->addEntity(other);

    Entity* d = new Entity();
    d->addComponent(new Driver());
    scene->addEntity(d);
}

int main() {
    Pancake::load(init);
    Pancake::start();
    CHECK(faded > 0 && frame == faded + 3);
    std::printf("engine ran %d frames, %d failed checks\n", frame, PancakeTest::failures());
    return PancakeTest::failures() == 0 ? 0 : 1;
}
