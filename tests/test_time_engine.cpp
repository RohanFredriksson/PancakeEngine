// Runs the real engine loop to check pausing, stepping and time scale, the console commands that go with them,
// colour tweens, and display settings. The driver counts frames, which keep coming while the scene is paused.

#include <cstdio>
#include "test.hpp"
#include "pancake/pancake.hpp"

using namespace Pancake;

namespace {

    int frame = 0;
    float pausedY = 0.0f;
    float steppedY = 0.0f;
    bool timerFired = false;
    int spawnedId = -1;
    SpriteRenderer* removedSprite = nullptr;

    void log(const char* message) {
        std::printf("frame %d: %s\n", frame, message);
        std::fflush(stdout);
    }

    Entity* box() {
        return Pancake::getScene()->findEntity("Box");
    }

}

class Driver : public Component {

    public:

        Driver() : Component("Driver") {}

        void update(float dt) override {

            frame++;
            Scene* scene = Pancake::getScene();

            if (frame == 5) {
                log("pause");
                Pancake::pause(true);
            }

            if (frame == 7) {
                CHECK(dt == 0.0f);
                pausedY = box()->getPosition().y;
                scene->after(0.001f, []() {timerFired = true;});
            }

            if (frame == 20) {
                log("nothing moves or fires while paused");
                CHECK(Pancake::isPaused());
                CHECK(box()->getPosition().y == pausedY);
                CHECK(!timerFired);
                CHECK(Pancake::getFps() > 0.0f);
                CHECK(Pancake::getUnscaledDeltaTime() > 0.0f);
                Pancake::step();
            }

            if (frame == 21) {
                CHECK_NEAR(dt, scene->getPhysics()->getTimeStep(), 1e-6);
            }

            if (frame == 22) {
                log("a step advances exactly one physics step and stays paused");
                steppedY = box()->getPosition().y;
                CHECK(steppedY < pausedY);
                CHECK(timerFired);
                CHECK(Pancake::isPaused());
                CHECK(dt == 0.0f);
            }

            if (frame == 25) {
                CHECK(box()->getPosition().y == steppedY);
                Console::execute("pause off");
                CHECK(!Pancake::isPaused());
                Console::execute("timescale 0");
                CHECK(Pancake::getTimeScale() == 0.0f);
            }

            if (frame == 26) {steppedY = box()->getPosition().y;}

            if (frame == 30) {
                log("a time scale of zero freezes the scene");
                CHECK(box()->getPosition().y == steppedY);
                Console::execute("timescale 2");
                CHECK(Pancake::getTimeScale() == 2.0f);
                Console::execute("timescale -1"); // Rejected.
                CHECK(Pancake::getTimeScale() == 2.0f);
                Console::execute("timescale 1");
            }

            if (frame == 31) {
                log("spawn, teleport and kill from the console");
                Console::execute("spawn 5 5 2 1");
                std::vector<Entity*> bodies = scene->getEntitiesWith<Rigidbody>();
                CHECK(bodies.size() == 2);
                for (Entity* e : bodies) {if (e->getName() != "Box") {spawnedId = e->getId();}}
                Entity* spawned = scene->getEntity(spawnedId);
                CHECK(spawned != nullptr);
                if (spawned != nullptr) {
                    CHECK_NEAR(spawned->getSize().x, 2.0, 1e-6);
                    CHECK_NEAR(spawned->getComponent<Rigidbody>()->getMass(), 2.0, 1e-6);
                    CHECK(spawned->hasComponent<SpriteRenderer>());
                }

                Console::execute("tp Box 0 100");
                CHECK_NEAR(box()->getPosition().y, 100.0, 1e-6);
                CHECK(box()->getComponent<Rigidbody>()->getVelocity() == vec2(0.0f, 0.0f));

                Console::execute("kill " + std::to_string(spawnedId));
                CHECK(scene->getEntity(spawnedId)->isDead());
                Console::execute("kill Box");
                CHECK(box() == nullptr);
                Console::execute("kill Nobody"); // Prints an error rather than crashing.

                Console::execute("fps on");
                Console::execute("vsync off");
                CHECK(!Window::hasVsync());
            }

            if (frame == 32) {
                log("colour tweens, including one whose sprite is removed");
                Entity* e = new Entity(vec2(-3.0f, 0.0f));
                SpriteRenderer* sprite = e->addComponent<SpriteRenderer>();
                sprite->setColour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
                Tween::colour(sprite, vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.0f);
                e->setName("Tinted");
                scene->addEntity(e);

                Entity* f = new Entity(vec2(3.0f, 0.0f));
                removedSprite = f->addComponent<SpriteRenderer>();
                Tween::colour(removedSprite, vec4(1.0f), 10.0f);
                scene->addEntity(f);
            }

            if (frame == 34) {
                SpriteRenderer* sprite = scene->findEntity("Tinted")->getComponent<SpriteRenderer>();
                CHECK_NEAR(sprite->getColour().r, 1.0, 1e-6);
                CHECK(!scene->findEntity("Tinted")->hasComponent<Tween>());
                removedSprite->kill(); // Deleted this frame. ASan reports it if the tween still uses it.
            }

            if (frame == 36) {
                log("fullscreen on");
                Console::execute("fullscreen on");
                CHECK(Pancake::isFullscreen());
            }

            if (frame == 40) {
                log("fullscreen off");
                Pancake::fullscreen(false);
                CHECK(!Pancake::isFullscreen());
            }

            if (frame == 45) {Pancake::stop();}

        }

};

void init(Scene* scene) {
    scene->getPhysics()->setGravity(vec2(0.0f, -10.0f));
    Entity* e = new Entity(vec2(0.0f, 0.0f));
    e->setName("Box");
    e->addComponent<SpriteRenderer>();
    BoxCollider* collider = new BoxCollider();
    collider->setMass(1.0f);
    e->addComponent<Rigidbody>()->addCollider(collider);
    scene->addEntity(e);
    Entity* d = new Entity();
    d->addComponent(new Driver());
    scene->addEntity(d);
}

int main() {
    Pancake::load(init);
    Pancake::start();
    CHECK(frame == 45);
    std::printf("engine ran %d frames, %d failed checks\n", frame, PancakeTest::failures());
    return PancakeTest::failures() == 0 ? 0 : 1;
}
