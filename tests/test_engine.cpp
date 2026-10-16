// Runs the real engine loop in a window and drives it through a script of frames:
// physics settling, saving and reloading a scene, killing bodies, resetting, and shutdown.

#include <cmath>
#include <cstdio>
#include "test.hpp"
#include "pancake/pancake.hpp"

using namespace Pancake;

namespace {

    const char* SAVE_FILE = "test_engine_scene.json";

    int frame = 0;
    int settled = -1; // The frame the box came to rest on, which later steps are timed from.
    int boxId = -1;
    int groundId = -1;

    void log(const char* message) {
        std::printf("frame %d: %s\n", frame, message);
        std::fflush(stdout);
    }

    Rigidbody* rigidbodyOf(int entityId) {
        Entity* e = Pancake::getEntity(entityId);
        if (e == nullptr) {return nullptr;}
        return dynamic_cast<Rigidbody*>(e->getComponent("Rigidbody"));
    }

    void addBody(Scene* scene, vec2 position, vec2 size, float mass, int* id) {
        Entity* e = new Entity(position);
        Rigidbody* r = new Rigidbody();
        BoxCollider* b = new BoxCollider();
        b->setSize(size);
        b->setMass(mass);
        r->addCollider(b);
        r->setRestitution(0.0f);
        e->addComponent(r);
        scene->addEntity(e);
        if (id != nullptr) {*id = e->getId();}
    }

}

// Drives the test. It is serialisable so it survives a save and reload of the scene.
class Driver : public Component {

    public:

        Driver() : Component("Driver") {}

        void update(float dt) override {

            frame++;

            // Physics runs in real time, so wait for the box to land rather than counting frames.
            if (settled < 0) {
                Rigidbody* box = rigidbodyOf(boxId);
                bool resting = box != nullptr && box->getEntity()->getPosition().y < -3.9f && std::fabs(box->getVelocity().y) < 0.05f;
                if (resting) {settled = frame;}
                else if (frame > 5000) {log("box never came to rest"); CHECK(false); Pancake::stop();}
                return;
            }

            int t = frame - settled;

            if (t == 30) {
                log("box should be resting on the ground");
                Rigidbody* box = rigidbodyOf(boxId);
                CHECK(box != nullptr);
                if (box != nullptr) {CHECK_NEAR(box->getEntity()->getPosition().y, -4.0, 0.1);}

                RaycastResult hit = Pancake::getScene()->getPhysics()->raycast(Ray(vec2(0.0f, 10.0f), vec2(0.0f, -1.0f)), 50.0f);
                CHECK(hit.hit != nullptr && hit.hit->getId() == boxId);

                std::vector<Entity*> near = Pancake::getScene()->getEntitiesNear(vec2(0.0f, -4.0f), 0.25f);
                bool found = false;
                for (Entity* e : near) {found = found || e->getId() == boxId;}
                CHECK(found);

                Pancake::save(SAVE_FILE);
            }

            if (t == 32) {
                log("reset and reload the saved scene");
                Pancake::reset();
                Pancake::load(SAVE_FILE);
            }

            if (t == 40) {
                log("the reloaded scene matches the saved one");
                CHECK(Pancake::getScene()->getName() == "Engine Test");
                CHECK_NEAR(Pancake::getScene()->getPhysics()->getGravity().y, -9.0, 1e-6);
                Rigidbody* box = rigidbodyOf(boxId);
                CHECK(box != nullptr);
                if (box != nullptr) {CHECK_NEAR(box->getEntity()->getPosition().y, -4.0, 0.1);}
                CHECK(rigidbodyOf(groundId) != nullptr);
            }

            if (t == 50) {
                log("kill the resting box while it touches the ground");
                Entity* box = Pancake::getEntity(boxId);
                CHECK(box != nullptr);
                if (box != nullptr) {box->kill();}
            }

            if (t == 60) {
                log("killed entities are removed from the scene");
                CHECK(Pancake::getEntity(boxId) == nullptr);
                CHECK(Pancake::getScene()->getEntitiesNear(vec2(0.0f, -4.0f), 0.25f).empty());
                Console::execute("debug on");
                CHECK(Pancake::getScene()->getPhysics()->hasDebugDraw());
                Console::execute("gravity 0 -3");
                CHECK_NEAR(Pancake::getScene()->getPhysics()->getGravity().y, -3.0, 1e-6);
            }

            if (t == 70) {
                log("reset a scene that has a force generator component");
                Pancake::reset();
                Pancake::load([](Scene* scene) {
                    Entity* e = new Entity();
                    Gravity* wind = new Gravity();
                    wind->setAcceleration(1.0f, 0.0f);
                    e->addComponent(wind);
                    scene->addEntity(e);
                    Entity* d = new Entity();
                    d->addComponent(new Driver());
                    scene->addEntity(d);
                });
            }

            if (t == 80) {
                log("reset again, then stop");
                Pancake::reset();
                Pancake::load([](Scene* scene) {
                    Entity* d = new Entity();
                    d->addComponent(new Driver());
                    scene->addEntity(d);
                });
            }

            if (t == 85) {Pancake::stop();}

        }

};

REGISTER(Component, Driver);

void init(Scene* scene) {
    scene->setName("Engine Test");
    scene->getPhysics()->setGravity(vec2(0.0f, -9.0f));
    addBody(scene, vec2(0.0f, -5.0f), vec2(10.0f, 1.0f), 0.0f, &groundId);
    addBody(scene, vec2(0.0f, 0.0f), vec2(1.0f, 1.0f), 1.0f, &boxId);
    Entity* d = new Entity();
    d->addComponent(new Driver());
    scene->addEntity(d);
}

int main() {
    Pancake::load(init);
    Pancake::start();
    std::remove(SAVE_FILE);
    CHECK(settled > 0 && frame == settled + 85);
    std::printf("engine ran %d frames, %d failed checks\n", frame, PancakeTest::failures());
    return PancakeTest::failures() == 0 ? 0 : 1;
}
