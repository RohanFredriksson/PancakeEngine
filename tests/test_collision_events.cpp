// Collision enter, contact and exit events, collision layers and masks, and raycast masks.

#include "test.hpp"
#include "helpers.hpp"

using namespace Pancake;
using PancakeTest::Bodies;

namespace {

    void step(World& world, int frames) {
        for (int i = 0; i < frames; i++) {world.update(1.0f / 60.0f);}
    }

    struct Recorder : public Component, public CollisionListener {

        int enters = 0;
        int contacts = 0;
        int exits = 0;
        Entity* lastEnter = nullptr;
        Entity* lastExit = nullptr;
        glm::vec2 enterNormal = glm::vec2(0.0f);
        bool killOnEnter = false;

        Recorder() : Component("Recorder") {}

        void collisionEnter(Entity* with, CollisionManifold manifold) override {
            enters++;
            lastEnter = with;
            enterNormal = manifold.normal;
            if (killOnEnter) {this->getEntity()->kill();}
        }

        void collision(Entity* with, CollisionManifold manifold) override {contacts++;}

        void collisionExit(Entity* with) override {
            exits++;
            lastExit = with;
        }

    };

    Recorder* record(Rigidbody* body) {
        Recorder* r = new Recorder();
        body->getEntity()->addComponent(r);
        return r;
    }

}

TEST("enter fires once, contacts every step, and exit when bodies separate") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, 0.0f));
    Bodies bodies;
    Rigidbody* ground = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(4.0f, 1.0f), 0.0f);
    Rigidbody* trigger = bodies.box(glm::vec2(0.0f, 0.9f), glm::vec2(1.0f, 1.0f), 1.0f);
    trigger->setSensor(true);
    Recorder* g = record(ground);
    Recorder* t = record(trigger);
    world.add(ground);
    world.add(trigger);

    step(world, 3);
    CHECK(g->enters == 1);
    CHECK(t->enters == 1);
    CHECK(g->lastEnter == trigger->getEntity());
    CHECK(t->lastEnter == ground->getEntity());
    CHECK_NEAR(g->enterNormal.y, -t->enterNormal.y, 1e-5); // Each side sees the normal pointing at the other.
    CHECK(g->contacts >= 3);
    CHECK(g->exits == 0);
    CHECK(world.isTouching(ground, trigger));
    CHECK(world.isTouching(trigger, ground));

    trigger->getEntity()->setPosition(glm::vec2(10.0f, 0.0f));
    step(world, 1);
    CHECK(g->exits == 1);
    CHECK(t->exits == 1);
    CHECK(g->lastExit == trigger->getEntity());
    CHECK(!world.isTouching(ground, trigger));
    int contacts = g->contacts;
    step(world, 2);
    CHECK(g->exits == 1);
    CHECK(g->contacts == contacts);

    trigger->getEntity()->setPosition(glm::vec2(0.0f, 0.9f));
    step(world, 1);
    CHECK(g->enters == 2);
}

TEST("removing a body sends exits to the bodies it touched, but not to itself") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, 0.0f));
    Bodies bodies;
    Rigidbody* a = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Rigidbody* b = bodies.box(glm::vec2(0.0f, 0.9f), glm::vec2(1.0f, 1.0f), 1.0f);
    b->setSensor(true);
    Recorder* ra = record(a);
    Recorder* rb = record(b);
    world.add(a);
    world.add(b);
    step(world, 1);
    CHECK(ra->enters == 1);

    b->getEntity()->kill();
    CHECK(!world.has(b));
    CHECK(ra->exits == 1);
    CHECK(ra->lastExit == b->getEntity());
    CHECK(rb->exits == 0); // Its entity is dying.
    step(world, 1);
    CHECK(ra->exits == 1);
}

TEST("listeners can kill bodies during a step, and exits match delivered enters") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, 0.0f));
    Bodies bodies;
    // A coin overlapping two players. The first enter collects the coin, so the second player never touches it.
    Rigidbody* player1 = bodies.box(glm::vec2(-0.6f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    Rigidbody* player2 = bodies.box(glm::vec2(0.6f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    Rigidbody* coin = bodies.circle(glm::vec2(0.0f, 0.0f), 0.3f, 1.0f);
    coin->setSensor(true);
    player1->setCollisionMask(ALL_LAYERS & ~1u); // Players don't push each other apart.
    player2->setCollisionMask(ALL_LAYERS & ~1u);
    player1->setCollisionLayer(1);
    player2->setCollisionLayer(1);
    coin->setCollisionLayer(2);
    Recorder* p1 = record(player1);
    Recorder* p2 = record(player2);
    Recorder* c = record(coin);
    c->killOnEnter = true;
    world.add(player1);
    world.add(player2);
    world.add(coin);

    step(world, 1);
    CHECK(c->enters == 1);
    CHECK(!world.has(coin));
    CHECK(p1->enters + p2->enters == 1);
    CHECK(p1->exits == p1->enters);
    CHECK(p2->exits == p2->enters);
    step(world, 2);
    CHECK(p1->enters + p2->enters == 1);
}

TEST("collision layers and masks filter pairs in both directions") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, 0.0f));
    Bodies bodies;
    Rigidbody* a = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    Rigidbody* b = bodies.box(glm::vec2(0.0f, 0.9f), glm::vec2(1.0f, 1.0f), 1.0f);
    CHECK(a->getCollisionLayer() == 1);
    CHECK(a->getCollisionMask() == ALL_LAYERS);
    CHECK(a->canCollideWith(b));

    a->setCollisionLayer(1 << 1);
    b->setCollisionMask(1 << 2); // b ignores a's layer, so neither collides, whatever a's mask is.
    CHECK(!a->canCollideWith(b));
    CHECK(!b->canCollideWith(a));

    Recorder* ra = record(a);
    world.add(a);
    world.add(b);
    step(world, 2);
    CHECK(ra->enters == 0);
    CHECK_NEAR(b->getEntity()->getPosition().y, 0.9, 1e-6); // Nothing pushed them apart.

    b->setCollisionMask((1 << 1) | (1 << 2));
    CHECK(a->canCollideWith(b));
    step(world, 1);
    CHECK(ra->enters == 1);
    CHECK(b->getEntity()->getPosition().y > 0.9f);
}

TEST("raycasts skip bodies outside the mask") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, 0.0f));
    Bodies bodies;
    Rigidbody* near = bodies.box(glm::vec2(3.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Rigidbody* far = bodies.box(glm::vec2(6.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    near->setCollisionLayer(1 << 3);
    world.add(near);
    world.add(far);
    Ray ray(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f));

    CHECK(world.raycast(ray, 20.0f).hit == near->getEntity());
    CHECK(world.raycast(ray).hit == near->getEntity());
    CHECK(world.raycast(ray, 20.0f, ALL_LAYERS & ~(1u << 3)).hit == far->getEntity());
    CHECK(world.raycast(ray, INFINITY, 1).hit == far->getEntity());
    CHECK(world.raycast(ray, 20.0f, 1 << 4).hit == nullptr);
}

TEST("collision layers and masks round trip, and are optional in older files") {
    Entity e;
    Rigidbody* r = new Rigidbody();
    r->setCollisionLayer(1 << 5)->setCollisionMask(0xF0F0F0F0);
    e.addComponent(r);
    nlohmann::json j = r->serialise();
    Rigidbody loaded;
    CHECK(loaded.load(j));
    CHECK(loaded.getCollisionLayer() == (1u << 5));
    CHECK(loaded.getCollisionMask() == 0xF0F0F0F0);

    j.erase("collisionLayer");
    j.erase("collisionMask");
    Rigidbody older;
    CHECK(older.load(j));
    CHECK(older.getCollisionLayer() == 1);
    CHECK(older.getCollisionMask() == ALL_LAYERS);
}

RUN_TESTS()
