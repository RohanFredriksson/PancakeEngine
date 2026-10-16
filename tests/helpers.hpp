#pragma once

// Helpers for building physics bodies outside of a running engine.

#include "pancake/pancake.hpp"

namespace PancakeTest {

    // Owns entities created for a test and deletes them at the end of scope.
    struct Bodies {

        std::vector<Pancake::Entity*> entities;

        ~Bodies() {
            for (Pancake::Entity* e : entities) {delete e;}
        }

        Pancake::Rigidbody* box(glm::vec2 position, glm::vec2 size, float mass, float rotation = 0.0f) {
            Pancake::Entity* e = new Pancake::Entity(position, glm::vec2(1.0f, 1.0f), rotation);
            Pancake::Rigidbody* r = new Pancake::Rigidbody();
            Pancake::BoxCollider* b = new Pancake::BoxCollider();
            b->setSize(size);
            b->setMass(mass);
            r->addCollider(b);
            e->addComponent(r);
            entities.push_back(e);
            return r;
        }

        Pancake::Rigidbody* circle(glm::vec2 position, float radius, float mass) {
            Pancake::Entity* e = new Pancake::Entity(position);
            Pancake::Rigidbody* r = new Pancake::Rigidbody();
            Pancake::CircleCollider* c = new Pancake::CircleCollider();
            c->setRadius(radius);
            c->setMass(mass);
            r->addCollider(c);
            e->addComponent(r);
            entities.push_back(e);
            return r;
        }

    };

}
