// Camera shake, CameraFollow and gamepad input, with no window.

#include <cmath>
#include "test.hpp"
#include "pancake/pancake.hpp"

using namespace Pancake;

TEST("shake moves the view within its intensity, then fades to nothing") {
    Camera camera(glm::vec2(1.0f, 2.0f), glm::vec2(12.0f, 12.0f), 1.0f);
    glm::mat4 still = camera.getView();
    camera.shake(0.5f, 1.0f);
    CHECK(camera.isShaking());

    bool moved = false;
    for (int i = 0; i < 30; i++) {
        camera.update(1.0f / 60.0f);
        glm::vec2 offset = camera.getShakeOffset();
        CHECK(std::fabs(offset.x) <= 0.5f && std::fabs(offset.y) <= 0.5f);
        moved = moved || offset != glm::vec2(0.0f);
    }
    CHECK(moved);
    CHECK(camera.getPosition() == glm::vec2(1.0f, 2.0f)); // The logical position never moves.
    CHECK(camera.getView() != still);

    // A weaker shake doesn't cut a stronger one short.
    camera.shake(0.01f, 5.0f);
    for (int i = 0; i < 40; i++) {camera.update(1.0f / 60.0f);}
    CHECK(!camera.isShaking());
    CHECK(camera.getShakeOffset() == glm::vec2(0.0f));
    CHECK(camera.getView() == still);
}

TEST("shake holds still while paused and can be stopped") {
    Camera camera(glm::vec2(0.0f), glm::vec2(12.0f, 12.0f), 1.0f);
    camera.shake(1.0f, 1.0f);
    camera.update(0.1f);
    glm::vec2 offset = camera.getShakeOffset();
    camera.update(0.0f);
    CHECK(camera.getShakeOffset() == offset);
    camera.stopShake();
    CHECK(!camera.isShaking());
    CHECK(camera.getShakeOffset() == glm::vec2(0.0f));
}

TEST("follow snaps first, then smooths independently of frame rate") {
    Entity target(glm::vec2(5.0f, 5.0f));
    CameraFollow follow;
    follow.setSmoothing(0.5f);
    Camera camera(glm::vec2(0.0f), glm::vec2(12.0f, 12.0f), 1.0f);

    follow.apply(&camera, &target, 1.0f / 60.0f);
    CHECK_NEAR(camera.getPosition().x, 5.0, 1e-5);

    // Half a second at 60 or 30 frames per second closes the same fraction of the gap.
    Camera fast(glm::vec2(5.0f, 5.0f), glm::vec2(12.0f, 12.0f), 1.0f);
    Camera slow(glm::vec2(5.0f, 5.0f), glm::vec2(12.0f, 12.0f), 1.0f);
    target.setPosition(glm::vec2(15.0f, 5.0f));
    for (int i = 0; i < 30; i++) {follow.apply(&fast, &target, 1.0f / 60.0f);}
    for (int i = 0; i < 15; i++) {follow.apply(&slow, &target, 1.0f / 30.0f);}
    double expected = 15.0 - 10.0 * std::exp(-1.0);
    CHECK_NEAR(fast.getPosition().x, expected, 1e-3);
    CHECK_NEAR(slow.getPosition().x, expected, 1e-3);

    // A paused scene doesn't move a smoothed camera.
    follow.apply(&fast, &target, 0.0f);
    CHECK_NEAR(fast.getPosition().x, expected, 1e-3);
}

TEST("follow ignores movement inside the dead zone and adds the offset") {
    Entity target(glm::vec2(0.0f, 0.0f));
    CameraFollow follow;
    follow.setDeadZone(glm::vec2(2.0f, 1.0f))->setOffset(glm::vec2(0.0f, 1.0f));
    Camera camera(glm::vec2(0.0f, 1.0f), glm::vec2(12.0f, 12.0f), 1.0f);
    follow.apply(&camera, &target, 0.1f);
    CHECK(camera.getPosition() == glm::vec2(0.0f, 1.0f));

    target.setPosition(glm::vec2(1.5f, -0.5f)); // Still inside.
    follow.apply(&camera, &target, 0.1f);
    CHECK(camera.getPosition() == glm::vec2(0.0f, 1.0f));

    target.setPosition(glm::vec2(5.0f, 0.0f)); // Out to the right, so the camera moves until it is on the edge.
    follow.apply(&camera, &target, 0.1f);
    CHECK_NEAR(camera.getPosition().x, 3.0, 1e-5);
    CHECK_NEAR(camera.getPosition().y, 1.0, 1e-5);
}

TEST("follow keeps the view inside its bounds, centring it when it doesn't fit") {
    Entity target(glm::vec2(0.0f, 0.0f));
    CameraFollow follow;
    Camera camera(glm::vec2(0.0f), glm::vec2(12.0f, 12.0f), 1.0f); // A 12 by 12 view, as the test window is square.
    follow.setBounds(glm::vec2(-20.0f, -3.0f), glm::vec2(20.0f, 3.0f));

    target.setPosition(glm::vec2(-100.0f, 0.0f));
    follow.apply(&camera, &target, 0.1f);
    CHECK_NEAR(camera.getPosition().x, -14.0, 1e-4); // Left edge of the view on the bound.
    CHECK_NEAR(camera.getPosition().y, 0.0, 1e-4);   // Taller than the bounds, so centred.

    camera.setZoom(2.0f); // Half the view, so it fits vertically now.
    target.setPosition(glm::vec2(0.0f, 50.0f));
    follow.apply(&camera, &target, 0.1f);
    CHECK_NEAR(camera.getPosition().y, 0.0, 1e-4);
    CHECK_NEAR(camera.getPosition().x, 0.0, 1e-4);
}

TEST("follow settings round trip through json") {
    CameraFollow original;
    original.setTarget(42)->setOffset(glm::vec2(1.0f, 2.0f))->setSmoothing(0.25f)->setDeadZone(glm::vec2(1.0f, 0.5f));
    original.setBounds(glm::vec2(5.0f, 5.0f), glm::vec2(-5.0f, -5.0f));
    Component* c = FACTORY(Component).create("CameraFollow");
    CHECK(c != nullptr);
    if (c == nullptr) {return;}
    CHECK(c->load(original.serialise()));
    CameraFollow* loaded = dynamic_cast<CameraFollow*>(c);
    CHECK(loaded->getTarget() == 42);
    CHECK(loaded->getOffset() == glm::vec2(1.0f, 2.0f));
    CHECK_NEAR(loaded->getSmoothing(), 0.25, 1e-6);
    CHECK(loaded->hasBounds());
    CHECK(loaded->getBoundsMin() == glm::vec2(-5.0f, -5.0f)); // Corners are ordered.
    CHECK(loaded->serialise() == original.serialise());
    nlohmann::json broken = original.serialise();
    broken.erase("deadZone");
    CHECK(!loaded->load(broken));
    delete c;
}

TEST("gamepad buttons begin and end down for one poll") {
    GLFWgamepadstate state = {};
    CHECK(!GamepadListener::isConnected(0));
    GamepadListener::setState(0, true, state, "Test Pad");
    CHECK(GamepadListener::isConnected(0));
    CHECK(GamepadListener::getName(0) == "Test Pad");

    state.buttons[GLFW_GAMEPAD_BUTTON_A] = GLFW_PRESS;
    GamepadListener::setState(0, true, state);
    CHECK(GamepadListener::isButtonDown(GLFW_GAMEPAD_BUTTON_A));
    CHECK(GamepadListener::isButtonBeginDown(GLFW_GAMEPAD_BUTTON_A));
    GamepadListener::setState(0, true, state);
    CHECK(GamepadListener::isButtonDown(GLFW_GAMEPAD_BUTTON_A));
    CHECK(!GamepadListener::isButtonBeginDown(GLFW_GAMEPAD_BUTTON_A));

    // Disconnecting releases every button.
    GamepadListener::setState(0, false, state);
    CHECK(!GamepadListener::isConnected(0));
    CHECK(!GamepadListener::isButtonDown(GLFW_GAMEPAD_BUTTON_A));
    CHECK(GamepadListener::isButtonEndDown(GLFW_GAMEPAD_BUTTON_A));
    GamepadListener::setState(0, false, state);
    CHECK(!GamepadListener::isButtonEndDown(GLFW_GAMEPAD_BUTTON_A));

    CHECK(!GamepadListener::isButtonDown(GLFW_GAMEPAD_BUTTON_A, 99));
    CHECK(!GamepadListener::isButtonDown(99));
}

TEST("gamepad axes remove the dead zone, flip stick y, and map triggers to 0 to 1") {
    GamepadListener::setDeadZone(0.2f);
    GLFWgamepadstate state = {};
    state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] = 0.1f;
    state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] = -1.0f; // GLFW reports up as negative.
    state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] = 0.6f;
    state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = -1.0f;
    state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = 1.0f;
    GamepadListener::setState(1, true, state);

    CHECK_NEAR(GamepadListener::getAxis(GLFW_GAMEPAD_AXIS_LEFT_X, 1), 0.0, 1e-6);
    CHECK_NEAR(GamepadListener::getAxis(GLFW_GAMEPAD_AXIS_RIGHT_X, 1), 0.5, 1e-6);
    CHECK_NEAR(GamepadListener::getAxis(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, 1), 0.0, 1e-6);
    CHECK_NEAR(GamepadListener::getAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, 1), 1.0, 1e-6);

    glm::vec2 left = GamepadListener::getLeftStick(1);
    CHECK(left.y > 0.99f);
    CHECK(left.x > 0.0f && left.x < 0.2f); // The dead zone applies to the stick's length, not each axis.
    CHECK_NEAR(GamepadListener::getRightStick(1).x, 0.5, 1e-6);

    GamepadListener::setState(1, false, state);
    CHECK(GamepadListener::getLeftStick(1) == glm::vec2(0.0f));
    CHECK_NEAR(GamepadListener::getAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, 1), 0.0, 1e-6);
}

RUN_TESTS()
