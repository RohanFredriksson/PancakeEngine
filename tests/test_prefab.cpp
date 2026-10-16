// Cloning entities, and saving and loading prefabs, with no window.

#include <cstdio>
#include <fstream>
#include <filesystem>
#include "test.hpp"
#include "pancake/pancake.hpp"

using namespace Pancake;

namespace {

    const char* PREFAB_FILE = "test_prefab_crate.json";

    // A component that isn't saved, to check clones and prefabs leave it out.
    struct Scratch : public Component {
        Scratch() : Component("Scratch") {this->setSerialisable(false);}
    };

    Entity* crate() {
        Entity* e = new Entity(glm::vec2(1.0f, 2.0f), glm::vec2(2.0f, 1.0f), 0.25f);
        e->setName("Crate");
        BoxCollider* box = new BoxCollider();
        box->setSize(2.0f, 1.0f);
        box->setMass(3.0f);
        e->addComponent<Rigidbody>()->addCollider(box)->setCollisionLayer(1 << 4);
        e->addComponent<Gravity>()->setAcceleration(0.0f, -2.0f);
        e->addComponent<Scratch>();
        return e;
    }

}

TEST("clones copy saved state with new entity and component ids") {
    Entity* original = crate();
    Entity* copy = original->clone();
    CHECK(copy != nullptr);
    if (copy == nullptr) {delete original; return;}

    CHECK(copy->getId() != original->getId());
    CHECK(copy->getName() == "Crate");
    CHECK(copy->getPosition() == original->getPosition());
    CHECK_NEAR(copy->getRotation(), 0.25, 1e-6);
    CHECK(copy->getComponents<Component>().size() == 2); // The unsaved component is left out.
    CHECK(!copy->hasComponent<Scratch>());

    Rigidbody* r = copy->getComponent<Rigidbody>();
    CHECK(r != nullptr);
    if (r != nullptr) {
        CHECK(r->getId() != original->getComponent<Rigidbody>()->getId());
        CHECK_NEAR(r->getMass(), 3.0, 1e-6);
        CHECK(r->getCollisionLayer() == (1u << 4));
        CHECK(r->getEntity() == copy);
    }

    // Ids are new, and later entities don't collide with them.
    Entity later;
    CHECK(later.getId() != copy->getId());
    CHECK(copy->getComponent<Gravity>()->getId() != original->getComponent<Gravity>()->getId());

    delete copy;
    delete original;
}

TEST("load keeps json ids while instantiate makes new ones") {
    Entity* original = crate();
    nlohmann::json j = original->serialise();
    Entity* loaded = Entity::load(j);
    Entity* instance = Entity::instantiate(j);
    CHECK(loaded->getId() == original->getId());
    CHECK(instance->getId() != original->getId());
    CHECK(instance->getId() != loaded->getId());
    nlohmann::json bad = j;
    bad.erase("position");
    CHECK(Entity::instantiate(bad) == nullptr);
    delete instance;
    delete loaded;
    delete original;
}

TEST("prefabs save, load as new copies, and are read again when the file changes") {
    Entity* original = crate();
    CHECK(Prefab::save(original, PREFAB_FILE));

    Entity* a = Prefab::load(PREFAB_FILE);
    Entity* b = Prefab::load(PREFAB_FILE);
    CHECK(a != nullptr && b != nullptr);
    if (a == nullptr || b == nullptr) {delete original; return;}
    CHECK(a->getId() != b->getId());
    CHECK(a->getId() != original->getId());
    CHECK(a->getPosition() == glm::vec2(1.0f, 2.0f));
    CHECK_NEAR(a->getComponent<Rigidbody>()->getMass(), 3.0, 1e-6);

    // Change the file, with a later modified time even on file systems with coarse timestamps.
    original->setName("Heavy Crate");
    CHECK(Prefab::save(original, PREFAB_FILE));
    std::filesystem::last_write_time(PREFAB_FILE, std::filesystem::last_write_time(PREFAB_FILE) + std::chrono::seconds(2));
    Entity* c = Prefab::load(PREFAB_FILE);
    CHECK(c != nullptr && c->getName() == "Heavy Crate");

    delete a;
    delete b;
    delete c;
    delete original;
    std::remove(PREFAB_FILE);
}

TEST("missing and malformed prefabs load as nullptr") {
    CHECK(Prefab::load("no_such_prefab.json") == nullptr);
    {
        std::ofstream file(PREFAB_FILE);
        file << "{ not json";
    }
    Prefab::clearCache();
    CHECK(Prefab::load(PREFAB_FILE) == nullptr);
    {
        std::ofstream file(PREFAB_FILE);
        file << "{\"id\": 1}";
    }
    std::filesystem::last_write_time(PREFAB_FILE, std::filesystem::last_write_time(PREFAB_FILE) + std::chrono::seconds(4));
    CHECK(Prefab::load(PREFAB_FILE) == nullptr);
    std::remove(PREFAB_FILE);
}

RUN_TESTS()
