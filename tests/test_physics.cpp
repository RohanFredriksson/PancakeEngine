#include "test.hpp"
#include "helpers.hpp"

using namespace Pancake;
using PancakeTest::Bodies;

namespace {

    void step(World& world, int frames) {
        for (int i = 0; i < frames; i++) {world.update(1.0f / 60.0f);}
    }

}

TEST("box/box overlap produces a contact along the separating axis") {
    Bodies bodies;
    Rigidbody* a = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(2.0f, 2.0f), 1.0f);
    Rigidbody* b = bodies.box(glm::vec2(0.0f, 1.9f), glm::vec2(2.0f, 2.0f), 1.0f);
    std::vector<CollisionManifold> m = Collision::findCollisionFeatures(a->getColliders()[0], b->getColliders()[0]);
    CHECK(!m.empty());
    for (CollisionManifold& c : m) {
        CHECK_NEAR(std::fabs(c.normal.y), 1.0, 1e-3);
        CHECK_NEAR(c.depth, 0.1, 1e-3);
    }
}

TEST("separated shapes do not collide") {
    Bodies bodies;
    Rigidbody* a = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    Rigidbody* b = bodies.box(glm::vec2(3.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    Rigidbody* c = bodies.circle(glm::vec2(0.0f, 3.0f), 0.5f, 1.0f);
    CHECK(Collision::findCollisionFeatures(a->getColliders()[0], b->getColliders()[0]).empty());
    CHECK(Collision::findCollisionFeatures(a->getColliders()[0], c->getColliders()[0]).empty());
}

TEST("circle/circle and circle/box overlaps are detected") {
    Bodies bodies;
    Rigidbody* a = bodies.circle(glm::vec2(0.0f, 0.0f), 1.0f, 1.0f);
    Rigidbody* b = bodies.circle(glm::vec2(1.5f, 0.0f), 1.0f, 1.0f);
    Rigidbody* c = bodies.box(glm::vec2(0.0f, -1.4f), glm::vec2(4.0f, 1.0f), 1.0f);
    std::vector<CollisionManifold> ab = Collision::findCollisionFeatures(a->getColliders()[0], b->getColliders()[0]);
    CHECK(ab.size() == 1);
    if (!ab.empty()) {CHECK_NEAR(ab[0].depth, 0.5, 1e-3);}
    CHECK(!Collision::findCollisionFeatures(a->getColliders()[0], c->getColliders()[0]).empty());
    CHECK(!Collision::findCollisionFeatures(c->getColliders()[0], a->getColliders()[0]).empty());
}

TEST("mass, centroid and moment of inertia combine colliders") {
    Entity e;
    Rigidbody* r = new Rigidbody();
    BoxCollider* left = new BoxCollider();
    BoxCollider* right = new BoxCollider();
    left->setSize(1.0f, 1.0f); left->setMass(1.0f); left->setPositionOffset(-1.0f, 0.0f);
    right->setSize(1.0f, 1.0f); right->setMass(3.0f); right->setPositionOffset(1.0f, 0.0f);
    r->addCollider(left)->addCollider(right);
    e.addComponent(r);
    CHECK_NEAR(r->getMass(), 4.0, 1e-5);
    CHECK_NEAR(r->getCentroid().x, 0.5, 1e-5);
    // Each box: m/6 about its centre, plus m * d^2 about the centroid.
    double expected = (1.0 / 6.0 + 1.0 * 1.5 * 1.5) + (3.0 / 6.0 + 3.0 * 0.5 * 0.5);
    CHECK_NEAR(r->getMomentOfInertia(), expected, 1e-4);
}

TEST("bounds wrap the colliders rather than the entity origin") {
    Bodies bodies;
    Rigidbody* r = bodies.box(glm::vec2(10.0f, 10.0f), glm::vec2(2.0f, 2.0f), 1.0f);
    r->getColliders()[0]->setPositionOffset(5.0f, 0.0f);
    std::pair<glm::vec2, glm::vec2> bounds = r->getBounds();
    CHECK_NEAR(bounds.first.x, 14.0, 1e-5);
    CHECK_NEAR(bounds.second.x, 16.0, 1e-5);
}

TEST("world gravity accelerates dynamic bodies and ignores static ones") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, -10.0f));
    Bodies bodies;
    Rigidbody* dynamic = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    Rigidbody* fixed = bodies.box(glm::vec2(10.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    world.add(dynamic);
    world.add(fixed);
    step(world, 60);
    CHECK_NEAR(dynamic->getVelocity().y, -10.0, 0.2);
    CHECK_NEAR(dynamic->getEntity()->getPosition().y, -5.0, 0.3);
    CHECK_NEAR(fixed->getEntity()->getPosition().y, 0.0, 1e-6);
}

TEST("gravity scale can disable gravity per body") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, -10.0f));
    Bodies bodies;
    Rigidbody* floating = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    floating->setGravityScale(0.0f);
    world.add(floating);
    step(world, 30);
    CHECK_NEAR(floating->getEntity()->getPosition().y, 0.0, 1e-6);
}

TEST("a falling box comes to rest on static ground") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, -10.0f));
    Bodies bodies;
    Rigidbody* ground = bodies.box(glm::vec2(0.0f, -5.0f), glm::vec2(10.0f, 1.0f), 0.0f);
    Rigidbody* box = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    box->setRestitution(0.0f);
    world.add(ground);
    world.add(box);
    step(world, 240);
    CHECK_NEAR(box->getEntity()->getPosition().y, -4.0, 0.05);
    CHECK_NEAR(box->getVelocity().y, 0.0, 0.5);
}

TEST("identical boxes stack without falling through each other") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, -10.0f));
    Bodies bodies;
    world.add(bodies.box(glm::vec2(0.0f, -0.5f), glm::vec2(1.0f, 1.0f), 0.0f));
    std::vector<Rigidbody*> stack;
    for (int i = 0; i < 3; i++) {
        Rigidbody* box = bodies.box(glm::vec2(0.0f, 0.5f + i * 1.0f), glm::vec2(1.0f, 1.0f), 1.0f);
        box->setRestitution(0.0f);
        world.add(box);
        stack.push_back(box);
    }
    step(world, 300);
    for (int i = 0; i < 3; i++) {
        CHECK_NEAR(stack[i]->getEntity()->getPosition().y, 0.5 + i, 0.05);
        CHECK_NEAR(stack[i]->getEntity()->getPosition().x, 0.0, 0.05);
    }
}

TEST("a pyramid of boxes stays standing") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, -10.0f));
    Bodies bodies;
    world.add(bodies.box(glm::vec2(0.0f, -4.5f), glm::vec2(20.0f, 1.0f), 0.0f));
    std::vector<Rigidbody*> boxes;
    std::vector<glm::vec2> expected;
    for (int row = 0; row < 5; row++) {
        for (int i = 0; i <= 4 - row; i++) {
            glm::vec2 position = glm::vec2(-2.0f + i + row * 0.5f, -3.5f + row);
            Rigidbody* box = bodies.box(position, glm::vec2(1.0f, 1.0f), 1.0f);
            box->setFriction(0.4f);
            box->setRestitution(0.2f);
            world.add(box);
            boxes.push_back(box);
            expected.push_back(position);
        }
    }
    step(world, 600);
    for (int i = 0; i < boxes.size(); i++) {
        CHECK(glm::distance(boxes[i]->getEntity()->getPosition(), expected[i]) < 0.05f);
        CHECK(std::fabs(boxes[i]->getEntity()->getRotation()) < 0.02f);
    }
}

TEST("a ball comes to rest on a static box without sinking") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, -10.0f));
    Bodies bodies;
    world.add(bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(20.0f, 1.0f), 0.0f, 0.0f));
    Rigidbody* ball = bodies.circle(glm::vec2(0.0f, 3.0f), 0.5f, 1.0f);
    ball->setRestitution(0.0f);
    world.add(ball);
    step(world, 240);
    CHECK_NEAR(ball->getEntity()->getPosition().y, 1.0, 0.05);
}

TEST("a box dropped at an angle settles flat on the ground") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, -10.0f));
    Bodies bodies;
    world.add(bodies.box(glm::vec2(0.0f, -5.0f), glm::vec2(20.0f, 1.0f), 0.0f));
    Rigidbody* box = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f, 0.4f);
    box->setRestitution(0.0f);
    box->setFriction(0.5f);
    world.add(box);
    step(world, 600);
    float rotation = box->getEntity()->getRotation();
    float quarter = 1.5707963f;
    float offFlat = std::fabs(rotation - quarter * std::round(rotation / quarter));
    CHECK(offFlat < 0.05f);
    CHECK_NEAR(box->getEntity()->getPosition().y, -4.0, 0.05);
    CHECK(std::fabs(box->getAngularVelocity()) < 0.1f);
}

TEST("a circle whose centre is inside a box is pushed out of the nearest face") {
    Bodies bodies;
    Rigidbody* box = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(4.0f, 2.0f), 0.0f);
    Rigidbody* circle = bodies.circle(glm::vec2(0.5f, 0.8f), 0.5f, 1.0f);
    std::vector<CollisionManifold> m = Collision::findCollisionFeatures(box->getColliders()[0], circle->getColliders()[0]);
    CHECK(m.size() == 1);
    if (!m.empty()) {
        CHECK_NEAR(m[0].normal.y, 1.0, 1e-4); // From the box to the circle.
        CHECK_NEAR(m[0].depth, 0.7, 1e-4);
    }
}

TEST("force generators only act on bodies registered with them") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, 0.0f));
    Bodies bodies;
    Gravity* wind = new Gravity();
    wind->setAcceleration(5.0f, 0.0f);
    Entity windEntity;
    windEntity.addComponent(wind);
    world.addForceGenerator(wind);
    Rigidbody* pushed = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    Rigidbody* ignored = bodies.box(glm::vec2(0.0f, 10.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    pushed->addForceGenerator("Gravity");
    world.add(pushed);
    world.add(ignored);
    step(world, 60);
    CHECK(pushed->getVelocity().x > 4.0f);
    CHECK_NEAR(ignored->getVelocity().x, 0.0, 1e-6);
    pushed->clearForceGenerators();
    glm::vec2 velocity = pushed->getVelocity();
    step(world, 10);
    CHECK_NEAR(pushed->getVelocity().x, velocity.x, 0.1);
    world.removeForceGenerator(wind);
}

TEST("removing a rigidbody also removes it from the spatial grid") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, 0.0f));
    Bodies bodies;
    Rigidbody* a = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    Rigidbody* b = bodies.box(glm::vec2(0.5f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    world.add(a);
    world.add(b);
    world.remove(a);
    CHECK(a->getWorld() == nullptr);
    for (std::vector<Rigidbody*>& cell : *world.getGrid()) {
        for (Rigidbody* r : cell) {CHECK(r != a);}
    }
    step(world, 5);
}

TEST("static bodies moved by game code update their grid cells") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, 0.0f));
    Bodies bodies;
    Rigidbody* wall = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    world.add(wall);
    wall->getEntity()->setPosition(glm::vec2(100.0f, 100.0f));
    step(world, 1);
    CHECK(world.getGrid()->get(0, 0).empty());
    CHECK(world.getGrid()->get(25, 25).count(wall) == 1);
}

TEST("physics drops time instead of spiralling after a long frame") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, -10.0f));
    Bodies bodies;
    Rigidbody* box = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    world.add(box);
    world.update(10.0f); // At most 5 steps.
    CHECK(box->getVelocity().y > -1.0f);
}

TEST("collision listeners are notified") {
    struct Listener : public Component, public CollisionListener {
        int hits = 0;
        Listener() : Component("Listener") {}
        void collision(Entity* with, CollisionManifold manifold) override {hits++;}
    };
    World world(1.0f / 60.0f, glm::vec2(0.0f, 0.0f));
    Bodies bodies;
    Rigidbody* a = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f);
    Rigidbody* b = bodies.box(glm::vec2(0.0f, 0.9f), glm::vec2(1.0f, 1.0f), 1.0f);
    Listener* listener = new Listener();
    a->getEntity()->addComponent(listener);
    b->setSensor(true);
    world.add(a);
    world.add(b);
    step(world, 1);
    CHECK(listener->hits > 0);
    CHECK_NEAR(b->getVelocity().y, 0.0, 1e-6); // Sensors receive no impulse.
}

RUN_TESTS()
