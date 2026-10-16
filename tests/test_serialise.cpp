#include "test.hpp"
#include "helpers.hpp"

using namespace Pancake;

TEST("entities with rigidbodies round trip through json") {
    Entity original(glm::vec2(1.0f, 2.0f), glm::vec2(3.0f, 4.0f), 0.5f);
    Rigidbody* r = new Rigidbody();
    BoxCollider* box = new BoxCollider();
    box->setSize(2.0f, 1.0f);
    box->setMass(2.0f);
    box->setPositionOffset(0.5f, 0.0f);
    CircleCollider* circle = new CircleCollider();
    circle->setRadius(0.75f);
    circle->setMass(1.0f);
    r->addCollider(box)->addCollider(circle);
    r->setFriction(0.3f)->setRestitution(0.2f)->setVelocity(1.0f, -1.0f);
    r->setGravityScale(0.5f);
    r->addForceGenerator("Wind");
    original.addComponent(r);

    nlohmann::json j = original.serialise();
    Entity* loaded = Entity::load(j);
    CHECK(loaded != nullptr);
    if (loaded == nullptr) {return;}

    CHECK(loaded->getId() == original.getId());
    CHECK_NEAR(loaded->getPosition().y, 2.0, 1e-6);
    CHECK_NEAR(loaded->getRotation(), 0.5, 1e-6);

    Rigidbody* lr = dynamic_cast<Rigidbody*>(loaded->getComponent("Rigidbody"));
    CHECK(lr != nullptr);
    if (lr != nullptr) {
        CHECK(lr->getColliders().size() == 2);
        CHECK_NEAR(lr->getMass(), 3.0, 1e-6);
        CHECK_NEAR(lr->getFriction(), 0.3, 1e-6);
        CHECK_NEAR(lr->getVelocity().x, 1.0, 1e-6);
        CHECK_NEAR(lr->getGravityScale(), 0.5, 1e-6);
        CHECK(lr->hasForceGenerator("Wind"));
        CHECK(lr->getColliders()[0]->getType() == "BoxCollider");
        CHECK(lr->getColliders()[1]->getType() == "CircleCollider");
    }

    // Serialising the loaded entity gives the same json.
    CHECK(loaded->serialise() == j);
    delete loaded;
}

TEST("force generator components round trip through json") {
    Gravity g;
    g.setAcceleration(1.0f, -3.0f);
    Component* c = FACTORY(Component).create("Gravity");
    CHECK(c != nullptr);
    if (c == nullptr) {return;}
    CHECK(c->load(g.serialise()));
    Gravity* loaded = dynamic_cast<Gravity*>(c);
    CHECK(loaded != nullptr);
    if (loaded != nullptr) {CHECK_NEAR(loaded->getAcceleration().y, -3.0, 1e-6);}
    delete c;
}

TEST("malformed json is rejected without crashing") {
    CHECK(Entity::load(nlohmann::json::object()) == nullptr);
    nlohmann::json j = {{"id", 1}, {"position", {0, 0}}, {"size", {1, 1}}, {"rotation", 0}, {"components", {{{"type", "NotAComponent"}}, {{"type", "Rigidbody"}}}}};
    Entity* e = Entity::load(j);
    CHECK(e != nullptr);
    if (e != nullptr) {CHECK(e->getComponents().empty()); delete e;}
}

RUN_TESTS()
