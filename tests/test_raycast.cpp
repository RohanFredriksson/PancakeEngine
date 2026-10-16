#include "test.hpp"
#include "helpers.hpp"

using namespace Pancake;
using PancakeTest::Bodies;

TEST("ray hits the near face of an axis aligned box") {
    Bodies bodies;
    Rigidbody* box = bodies.box(glm::vec2(5.0f, 0.0f), glm::vec2(2.0f, 2.0f), 1.0f);
    RaycastResult r = Raycast::raycast(box, Ray(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
    CHECK(r.hit == box->getEntity());
    CHECK_NEAR(r.distance, 4.0, 1e-4);
    CHECK_NEAR(r.point.x, 4.0, 1e-4);
    CHECK_NEAR(r.normal.x, -1.0, 1e-4);
}

TEST("ray misses a box beside it and boxes behind it") {
    Bodies bodies;
    Rigidbody* box = bodies.box(glm::vec2(5.0f, 3.0f), glm::vec2(2.0f, 2.0f), 1.0f);
    CHECK(Raycast::raycast(box, Ray(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f))).hit == nullptr);
    CHECK(Raycast::raycast(box, Ray(glm::vec2(0.0f, 3.0f), glm::vec2(-1.0f, 0.0f))).hit == nullptr);
}

TEST("ray hits the corner region of a rotated box") {
    Bodies bodies;
    // A unit-half-extent box rotated 45 degrees has its corner at x = 5 - sqrt(2).
    Rigidbody* box = bodies.box(glm::vec2(5.0f, 0.0f), glm::vec2(2.0f, 2.0f), 1.0f, 0.78539816f);
    RaycastResult r = Raycast::raycast(box, Ray(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
    CHECK(r.hit != nullptr);
    CHECK_NEAR(r.point.x, 5.0 - std::sqrt(2.0), 1e-3);
    CHECK(r.normal.x < 0.0f);
}

TEST("ray starting inside a box reports the exit point") {
    Bodies bodies;
    Rigidbody* box = bodies.box(glm::vec2(0.0f, 0.0f), glm::vec2(2.0f, 2.0f), 1.0f);
    RaycastResult r = Raycast::raycast(box, Ray(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
    CHECK_NEAR(r.distance, 1.0, 1e-4);
    CHECK_NEAR(r.normal.y, 1.0, 1e-4);
}

TEST("ray hits a circle and ignores circles behind it") {
    Bodies bodies;
    Rigidbody* circle = bodies.circle(glm::vec2(0.0f, 5.0f), 1.0f, 1.0f);
    RaycastResult r = Raycast::raycast(circle, Ray(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
    CHECK_NEAR(r.distance, 4.0, 1e-4);
    CHECK_NEAR(r.normal.y, -1.0, 1e-4);
    CHECK(Raycast::raycast(circle, Ray(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, -1.0f))).hit == nullptr);
}

TEST("world raycast returns the closest hit, with and without a max distance") {
    World world(1.0f / 60.0f, glm::vec2(0.0f, 0.0f));
    Bodies bodies;
    Rigidbody* nearBox = bodies.box(glm::vec2(10.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Rigidbody* farBox = bodies.box(glm::vec2(30.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Rigidbody* offAxis = bodies.circle(glm::vec2(5.0f, 10.0f), 1.0f, 0.0f);
    world.add(nearBox);
    world.add(farBox);
    world.add(offAxis);

    Ray ray(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
    CHECK(world.raycast(ray).hit == nearBox->getEntity());
    CHECK(world.raycast(ray, 100.0f).hit == nearBox->getEntity());
    CHECK(world.raycast(ray, 5.0f).hit == nullptr);

    Ray backwards(glm::vec2(35.0f, 0.0f), glm::vec2(-1.0f, 0.0f));
    CHECK(world.raycast(backwards, 100.0f).hit == farBox->getEntity());

    Ray diagonal(glm::vec2(0.0f, 5.0f), glm::vec2(1.0f, 1.0f));
    RaycastResult d = world.raycast(diagonal, 50.0f);
    CHECK(d.hit == offAxis->getEntity());
    CHECK_NEAR(d.distance, world.raycast(diagonal).distance, 1e-5);
}

RUN_TESTS()
