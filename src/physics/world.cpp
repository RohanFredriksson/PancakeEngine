#include <algorithm>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include "physics/world.hpp"
#include "graphics/renderer/debugdraw.hpp"

#include "physics/primitives/box.hpp"
#include "physics/primitives/circle.hpp"

namespace {
    
    const int IMPULSE_ITERATIONS =  6;

    void applyImpulse(Rigidbody* a, Rigidbody* b, CollisionManifold* m) {

        // Check for infinite mass objects.
        if (a->hasInfiniteMass() && b->hasInfiniteMass()) {return;}

        // If one rigidbody is static.
        if (a->hasInfiniteMass() || b->hasInfiniteMass()) {

            // Relative velocity
            vec2 relativeVelocity = b->getVelocity() - a->getVelocity();
            vec2 relativeNormal = glm::normalize(m->normal);

            // If moving away do nothing.
            if (glm::dot(relativeVelocity, relativeNormal) > 0.0f) {return;}

            float e = std::min(a->getCor(), b->getCor());
            float numerator = -(1.0f + e) * glm::dot(relativeVelocity, relativeNormal);
            float j = numerator / (a->hasInfiniteMass() ? 1.0f / b->getMass() : 1.0f / a->getMass());
            
            vec2 impulse = relativeNormal * j;
            if (a->hasInfiniteMass()) {

                vec2 btmp = impulse * (1.0f / b->getMass());
                b->addVelocity(btmp);

                // For static objects, move them outside of the obstacles, so they dont eventually sink through.
                btmp = m->normal * 0.5f * m->depth; // I don't know why 0.5f works.
                b->getEntity()->addPosition(btmp);

            }

            else {

                vec2 atmp = impulse * (-1.0f / a->getMass());
                a->addVelocity(atmp);

                // For static objects, move them outside of the obstacles, so they dont eventually sink through.
                atmp = m->normal * -0.5f * m->depth; // I don't know why 0.5f works.
                a->getEntity()->addPosition(atmp);

            }

        }

        else {

            // Linear velocity
            float invMass1 = 1.0f / a->getMass();
            float invMass2 = 1.0f / b->getMass();
            float invMassSum = invMass1 = invMass2;
            if (invMassSum == 0.0f) {return;}

            // Relative velocity
            vec2 relativeVelocity = b->getVelocity() - a->getVelocity();
            vec2 relativeNormal = glm::normalize(m->normal);

            // If moving away do nothing.
            if (glm::dot(relativeVelocity, relativeNormal) > 0.0f) {return;}

            float e = std::min(a->getCor(), b->getCor());
            float numerator = -(1.0f + e) * glm::dot(relativeVelocity, relativeNormal);
            float j = numerator / (a->hasInfiniteMass() ? 1.0f / b->getMass() : 1.0f / a->getMass());
            
            vec2 impulse = relativeNormal * j;

            vec2 atmp = impulse * -1.0f * invMass1;
            a->addVelocity(atmp);

            vec2 btmp = impulse * invMass2;
            b->addVelocity(btmp);

        }

    }

}

World::World(float timeStep, vec2 gravity) {
    this->gravity = new Gravity(vec2(0.0f, -10.0f));
    this->timeStep = timeStep;
    this->time = 0.0f;
}

World::~World() {
    delete this->gravity;
    this->clearCollisionLists();
}

void World::update(float dt) {
    this->time += dt;
    while (this->time >= 0.0f) {
        this->fixedUpdate();
        this->time -= this->timeStep;
    }
}

void World::fixedUpdate() {

    // Update velocities of all rigidbodies
    int n = this->rigidbodies.size();
    for (int i = 0; i < n; i++) {
        this->rigidbodies[i]->physicsUpdate(this->timeStep);
    }

    // Clear the collision lists.
    this->clearCollisionLists();

    // Find all collisions.
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            if (i == j) {continue;}

            Rigidbody* r1 = this->rigidbodies[i];
            Rigidbody* r2 = this->rigidbodies[j];

            Collider* c1 = r1->getCollider();
            Collider* c2 = r2->getCollider();

            CollisionManifold* result = NULL;
            if (c1 != NULL && c2 != NULL && !(r1->hasInfiniteMass() && r2->hasInfiniteMass())) {
                result = Collision::findCollisionFeatures(c1, c2);
            }

            if (result != NULL) {
            
                if (result->colliding) {

                    // Execute on collision
                    //c1->getEntity()->collision(c2->getEntity, result.contactPoint, result.normal);
                    //c2->getEntity()->collision(c1->getEntity, result.contactPoint, result.normal);

                    // If neither object is a trigger, do the collision.
                    if (!r1->isSensor() && !r2->isSensor()) {

                        this->bodies1.push_back(r1);
                        this->bodies2.push_back(r2);
                        this->collisions.push_back(result);

                    }

                } else {
                    delete result;
                }

            } 

        }

    }

    // Update the forces
    this->registry.updateForces(this->timeStep);

    // Resolve collision via iterative impulse resolution.
    for (int k = 0; k < IMPULSE_ITERATIONS; k++) {
        int m = this->collisions.size();
        for (int i = 0; i < m; i++) {
            Rigidbody* r1 = this->bodies1[i];
            Rigidbody* r2 = this->bodies2[i];
            applyImpulse(r1, r2, this->collisions[i]);
        }
    }

}

void World::clearCollisionLists() {
    
    int n = this->collisions.size();
    for (int i = 0; i < n; i++) {
        delete this->collisions[i];
    }
    this->collisions.clear();

    this->bodies1.clear();
    this->bodies2.clear();

}

void World::render() {

    int n = this->rigidbodies.size();
    for (int i = 0; i < n; i++) {

        Rigidbody* rigidbody = this->rigidbodies[i];
        Collider* collider = rigidbody->getCollider();
        if (collider == NULL) {continue;}
    
        vec2 position = rigidbody->getPosition();
        if (dynamic_cast<Box*>(collider) != nullptr) {
            Box* box = (Box*) collider;
            DebugDraw::drawBox(position, box->getSize(), /*box->getRotation()*/ 0.0f, vec3(0.0f, 1.0f, 0.0f), 1);
        }

        else if (dynamic_cast<Circle*>(collider) != nullptr) {
            Circle* circle = (Circle*) collider;
            DebugDraw::drawCircle(position, circle->getRadius(), vec3(0.0f, 1.0f, 0.0f), 1);
        }

    }

}

void World::add(Rigidbody* rigidbody) {
    this->rigidbodies.push_back(rigidbody);
    this->registry.add(this->gravity, rigidbody);
}

void World::remove(Rigidbody* rigidbody) {
    int n = this->rigidbodies.size();
    for (int i = 0; i < n; i++) {
        if (this->rigidbodies[i] == rigidbody) {
            this->registry.remove(this->gravity, rigidbody);
            this->rigidbodies.erase(this->rigidbodies.begin() + i);
            return;
        }
    }
}