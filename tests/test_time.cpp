// Scene timers, tweens and easing, driven with explicit delta times and no window.

#include <cmath>
#include "test.hpp"
#include "pancake/pancake.hpp"

using namespace Pancake;

TEST("one shot timers fire once after their delay") {
    Scene scene;
    int fired = 0;
    int timer = scene.after(0.5f, [&fired]() {fired++;});
    CHECK(scene.hasTimer(timer));
    scene.update(0.25f);
    CHECK(fired == 0);
    scene.update(0.25f);
    CHECK(fired == 1);
    CHECK(!scene.hasTimer(timer));
    scene.update(1.0f);
    CHECK(fired == 1);
}

TEST("repeating timers fire at most once a frame and keep their rhythm") {
    Scene scene;
    int fired = 0;
    int timer = scene.every(0.1f, [&fired]() {fired++;});
    for (int i = 0; i < 10; i++) {scene.update(0.05f);}
    CHECK(fired == 5);

    // A long frame fires once, then catches up over the next frames.
    scene.update(0.35f);
    CHECK(fired == 6);
    scene.update(0.0f);
    CHECK(fired == 7);

    scene.cancel(timer);
    scene.update(1.0f);
    CHECK(fired == 7);
    CHECK(!scene.hasTimer(timer));
}

TEST("a zero delta time freezes timers") {
    Scene scene;
    bool fired = false;
    scene.after(0.1f, [&fired]() {fired = true;});
    for (int i = 0; i < 100; i++) {scene.update(0.0f);}
    CHECK(!fired);
}

TEST("timers can add and cancel timers from their callbacks") {
    Scene scene;
    int first = 0;
    int second = 0;
    int repeating = scene.every(0.1f, [&]() {first++;});
    scene.after(0.1f, [&]() {
        scene.cancel(repeating);
        scene.after(0.1f, [&]() {second++;});
    });
    scene.update(0.1f);
    CHECK(first == 1); // The repeating timer was earlier in the list, so it fired before being cancelled.
    CHECK(second == 0);
    scene.update(0.1f);
    CHECK(first == 1);
    CHECK(second == 1);
}

TEST("timers with an owner are cancelled when it dies") {
    Scene scene;
    Entity* owner = new Entity();
    scene.addEntity(owner);
    bool fired = false;
    int timer = scene.after(0.1f, [&fired]() {fired = true;}, owner);
    owner->kill();
    scene.update(0.2f);
    CHECK(!fired);
    CHECK(!scene.hasTimer(timer));
}

TEST("easing functions start at 0 and end at 1") {
    Ease::Function functions[] = {Ease::linear, Ease::inQuad, Ease::outQuad, Ease::inOutQuad, Ease::inCubic, Ease::outCubic,
                                  Ease::inOutCubic, Ease::inSine, Ease::outSine, Ease::inOutSine, Ease::outBack, Ease::outBounce};
    for (Ease::Function f : functions) {
        CHECK_NEAR(f(0.0f), 0.0, 1e-5);
        CHECK_NEAR(f(1.0f), 1.0, 1e-5);
    }
    CHECK(Ease::outQuad(0.5f) > 0.5f);
    CHECK(Ease::inQuad(0.5f) < 0.5f);
}

TEST("position tweens ease from where the entity is when they begin") {
    Entity e(glm::vec2(0.0f, 0.0f));
    bool done = false;
    Tween* tween = Tween::position(&e, glm::vec2(10.0f, 0.0f), 1.0f, Ease::inQuad);
    tween->setDelay(0.5f)->onComplete([&done]() {done = true;});

    e.setPosition(glm::vec2(2.0f, 0.0f)); // Moved during the delay, so the tween starts from here.
    e.update(0.5f);
    CHECK_NEAR(e.getPosition().x, 2.0, 1e-5);
    e.update(0.5f);
    CHECK_NEAR(e.getPosition().x, 2.0 + 8.0 * 0.25, 1e-5);
    CHECK(!done);
    e.update(0.75f); // Overshooting the end lands exactly on the target.
    CHECK_NEAR(e.getPosition().x, 10.0, 1e-5);
    CHECK(done);
    CHECK(tween->isFinished());
    CHECK(!e.hasComponent<Tween>());
    e.update(0.0f); // The finished tween is deleted.
    CHECK(e.getComponents<Component>().empty());
}

TEST("tweens can be chained from onComplete") {
    Entity e(glm::vec2(0.0f, 0.0f));
    Tween::position(&e, glm::vec2(1.0f, 0.0f), 1.0f)->onComplete([&e]() {
        Tween::position(&e, glm::vec2(1.0f, 1.0f), 1.0f);
    });
    e.update(1.0f);
    CHECK_NEAR(e.getPosition().x, 1.0, 1e-5);
    e.update(0.5f);
    CHECK_NEAR(e.getPosition().y, 0.5, 1e-5);
    e.update(0.5f);
    CHECK_NEAR(e.getPosition().y, 1.0, 1e-5);
}

TEST("rotation and size tweens, and value tweens for anything else") {
    Entity e(glm::vec2(0.0f), glm::vec2(1.0f), 0.0f);
    Tween::rotation(&e, 1.0f, 1.0f);
    Tween::size(&e, glm::vec2(3.0f, 1.0f), 1.0f);
    float value = 0.0f;
    Tween::value(&e, 2.0f, [&value](float t) {value = 4.0f * t;});
    e.update(1.0f);
    CHECK_NEAR(e.getRotation(), 1.0, 1e-5);
    CHECK_NEAR(e.getSize().x, 3.0, 1e-5);
    CHECK_NEAR(value, 2.0, 1e-5);
}

RUN_TESTS()
