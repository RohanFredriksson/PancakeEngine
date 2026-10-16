// Entity and input APIs that need no window: typed component access, names, and key and mouse events.

#include "test.hpp"
#include "pancake/pancake.hpp"

using namespace Pancake;

TEST("typed component access matches the class and its subclasses") {
    Entity e;
    Rigidbody* r = e.addComponent<Rigidbody>();
    Gravity* g = e.addComponent<Gravity>();
    g->setAcceleration(0.0f, -3.0f);

    CHECK(r->getEntity() == &e);
    CHECK(e.getComponent<Rigidbody>() == r);
    CHECK(e.getComponent<Gravity>() == g);
    CHECK(e.getComponent<ForceGenerator>() == g);
    CHECK(e.hasComponent<Rigidbody>());
    CHECK(!e.hasComponent<SpriteRenderer>());
    CHECK(e.getComponent<SpriteRenderer>() == nullptr);
    CHECK(e.getComponents<Component>().size() == 2);
    CHECK(e.getComponents<ForceGenerator>().size() == 1);

    // Dead components are not returned, even before the update loop deletes them.
    g->kill();
    CHECK(!e.hasComponent<Gravity>());
    CHECK(e.getComponents<Component>().size() == 1);
}

TEST("entity names round trip through json and are optional") {
    Entity named(glm::vec2(1.0f, 2.0f));
    named.setName("Player");
    nlohmann::json j = named.serialise();
    CHECK(j["name"] == "Player");
    Entity* loaded = Entity::load(j);
    CHECK(loaded != nullptr);
    if (loaded != nullptr) {
        CHECK(loaded->getName() == "Player");
        CHECK(loaded->serialise() == j);
        delete loaded;
    }

    // Unnamed entities don't write a name, so older save files load unchanged.
    Entity unnamed;
    nlohmann::json k = unnamed.serialise();
    CHECK(!k.contains("name"));
    loaded = Entity::load(k);
    CHECK(loaded != nullptr);
    if (loaded != nullptr) {
        CHECK(loaded->getName().empty());
        delete loaded;
    }

    // A name that isn't a string is malformed.
    k["name"] = 5;
    CHECK(Entity::load(k) == nullptr);
}

TEST("keys report begin and end down for one frame") {
    KeyListener::keyCallback(nullptr, GLFW_KEY_SPACE, 0, GLFW_PRESS, 0);
    CHECK(KeyListener::isKeyDown(GLFW_KEY_SPACE));
    CHECK(KeyListener::isKeyBeginDown(GLFW_KEY_SPACE));
    CHECK(!KeyListener::isKeyEndDown(GLFW_KEY_SPACE));
    KeyListener::endFrame();
    CHECK(KeyListener::isKeyDown(GLFW_KEY_SPACE));
    CHECK(!KeyListener::isKeyBeginDown(GLFW_KEY_SPACE));

    KeyListener::keyCallback(nullptr, GLFW_KEY_SPACE, 0, GLFW_RELEASE, 0);
    CHECK(!KeyListener::isKeyDown(GLFW_KEY_SPACE));
    CHECK(KeyListener::isKeyEndDown(GLFW_KEY_SPACE));
    KeyListener::endFrame();
    CHECK(!KeyListener::isKeyEndDown(GLFW_KEY_SPACE));
    CHECK(!KeyListener::isKeyDown());
}

TEST("a key tapped within one frame still begins and ends down") {
    KeyListener::keyCallback(nullptr, GLFW_KEY_J, 0, GLFW_PRESS, 0);
    KeyListener::keyCallback(nullptr, GLFW_KEY_J, 0, GLFW_RELEASE, 0);
    CHECK(!KeyListener::isKeyDown(GLFW_KEY_J));
    CHECK(KeyListener::isKeyBeginDown(GLFW_KEY_J));
    CHECK(KeyListener::isKeyEndDown(GLFW_KEY_J));
    KeyListener::endFrame();
    CHECK(!KeyListener::isKeyBeginDown(GLFW_KEY_J));
    CHECK(!KeyListener::isKeyEndDown(GLFW_KEY_J));
    CHECK(!KeyListener::isKeyEndDown(-1));
}

TEST("mouse buttons report begin and end down for one frame") {
    MouseListener::mouseButtonCallback(nullptr, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
    CHECK(MouseListener::isMouseBeginDown(GLFW_MOUSE_BUTTON_LEFT));
    MouseListener::endFrame(); // Safe without a scene.
    CHECK(MouseListener::isMouseDown(GLFW_MOUSE_BUTTON_LEFT));
    CHECK(!MouseListener::isMouseBeginDown(GLFW_MOUSE_BUTTON_LEFT));

    MouseListener::mouseButtonCallback(nullptr, GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, 0);
    CHECK(!MouseListener::isMouseDown(GLFW_MOUSE_BUTTON_LEFT));
    CHECK(MouseListener::isMouseEndDown(GLFW_MOUSE_BUTTON_LEFT));
    MouseListener::endFrame();
    CHECK(!MouseListener::isMouseEndDown(GLFW_MOUSE_BUTTON_LEFT));
    CHECK(!MouseListener::isMouseEndDown(99));
}

RUN_TESTS()
