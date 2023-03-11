#include <limits.h>
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

            Rigidbody* rigidbody1 = this->rigidbodies[i];
            Rigidbody* rigidbody2 = this->rigidbodies[j];

            if (rigidbody1->hasInfiniteMass() && rigidbody2->hasInfiniteMass()) {continue;}

            vector<Collider*> colliders1 = rigidbody1->getColliders();
            vector<Collider*> colliders2 = rigidbody2->getColliders();

            if (colliders1.size() == 0 || colliders2.size() == 0) {continue;}

            vector<CollisionManifold*> tmp;
            bool cardinal = false;
            for (int k = 0; k < colliders1.size(); k++) {
                for (int l = 0; l < colliders2.size(); l++) {
                    
                    Collider* collider1 = colliders1[k];
                    Collider* collider2 = colliders2[l];

                    CollisionManifold* result = Collision::findCollisionFeatures(collider1, collider2);
                    if (result == NULL) {continue;}
                    if (result->isCardinal()) {cardinal = true;}
                    tmp.push_back(result);

                }
            }

            vector<CollisionManifold*> results;
            if (cardinal) {
                
                for (int k = 0; k < tmp.size(); k++) {
                    if (tmp[k]->isCardinal()) {results.push_back(tmp[k]);} 
                    else {delete tmp[k];}
                }
                
            } 
            
            else {

                for (int k = 0; k < tmp.size(); k++) {
                    results.push_back(tmp[k]);
                }

            }

            if (results.size() > 0) {

                rigidbody1->getEntity()->onCollision(rigidbody2);
                rigidbody2->getEntity()->onCollision(rigidbody1);

                this->bodies1.push_back(rigidbody1);
                this->bodies2.push_back(rigidbody2);
                this->collisions.push_back(results);

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
            for (int j = 0; j < this->collisions.size(); j++) {
                applyImpulse(r1, r2, this->collisions[i][j]);
            }
        }
    }

}

void World::clearCollisionLists() {
    
    int n = this->collisions.size();
    for (int i = 0; i < n; i++) {
        int m = this->collisions[i].size();
        for (int j = 0; j < m; j++) {
            delete this->collisions[i][j];
        }
    }

    this->collisions.clear();
    this->bodies1.clear();
    this->bodies2.clear();

}

void World::render() {

    int n = this->rigidbodies.size();
    for (int i = 0; i < n; i++) {

        Rigidbody* rigidbody = this->rigidbodies[i];
        vector<Collider*> colliders = rigidbody->getColliders();
    
        for (int j = 0; j < colliders.size(); j++) {

            Collider* collider = colliders[j];

            vec2 position = collider->getPosition();
            if (dynamic_cast<Box*>(collider) != nullptr) {
                Box* box = (Box*) collider;
                //DebugDraw::drawBox(position, box->getSize(), box->getRotation(), vec3(0.0f, 1.0f, 0.0f), 1);
                DebugDraw::drawBox(position, box->getSize(), 0.0f, vec3(0.0f, 1.0f, 0.0f), 1);
            }

            else if (dynamic_cast<Circle*>(collider) != nullptr) {
                Circle* circle = (Circle*) collider;
                DebugDraw::drawCircle(position, circle->getRadius(), vec3(0.0f, 1.0f, 0.0f), 1);
            }

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

RaycastResult World::raycast(Ray* ray) {

    bool hit = false;
    RaycastResult best;
    float bestDistance = FLT_MAX;

    int n = this->rigidbodies.size();
    for (int i = 0; i < n; i++) {
        RaycastResult current = Raycast::raycast(this->rigidbodies[i], ray);
        if (current.hit != NULL && current.distance < bestDistance) {
            bestDistance = current.distance;
            best = current;
            hit = true;
        }
    }

    if (hit) {return RaycastResult();}
    return best;

}