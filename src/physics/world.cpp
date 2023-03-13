#include <limits.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "physics/world.hpp"
#include "physics/primitives/box.hpp"
#include "physics/primitives/circle.hpp"
#include "graphics/renderer/debugdraw.hpp"

#include <iostream>

namespace {
    
    const int IMPULSE_ITERATIONS =  6;

    inline float cross(vec2 a, vec2 b) {
        return a.x * b.y + a.y * b.x;
    }

    inline vec2 cross(vec2 a, float s) {
        return vec2(s * a.y, -s * a.x);
    }

    inline vec2 cross(float s, vec2 a) {
        return vec2(-s * a.y, s * a.x);
    }

    vec2 tangentVelocity(vec2 radius, float angularVelocity) {
        return angularVelocity * glm::rotate(glm::normalize(radius), glm::radians(90.0f));
    }

    void applyImpulse(Rigidbody* a, Rigidbody* b, CollisionManifold* m) {

        DebugDraw::drawCircle(m->contactPoint, 0.1f, vec3(0.0f, 0.0f, 1.0f), 10);

        // Check for infinite mass objects.
        if (a->hasInfiniteMass() && b->hasInfiniteMass()) {return;}

        // Get the inverse masses.
        float aInvMass = a->getInverseMass();
        float bInvMass = b->getInverseMass();
        float sumInvMass = aInvMass + bInvMass;
        if (sumInvMass == 0.0f) {return;}

        // Calculate relative velocities of the bodies at the point of contact.
        vec2 r1 = m->contactPoint - a->getEntity()->getPosition();
        vec2 r2 = m->contactPoint - b->getEntity()->getPosition();
        vec2 relativeVelocity = b->getVelocity() - a->getVelocity();
        vec2 contactVelocity = glm::dot(relativeVelocity, m->normal) * m->normal;

        // If both the linear and angular velocity are moving away from the object then the collision has been resolved.
        if (glm::dot(relativeVelocity, m->normal) > 0.0f) {return;}

        // Calculate the impulse required to resolve this collision.
        float e = std::min(a->getCor(), b->getCor());
        vec2 impulse = (-(1.0f + e) * contactVelocity) / sumInvMass;

        // Apply impulses to the bodies proportionally to their mass.
        a->addVelocity(-impulse * a->getMass());
        b->addVelocity(impulse * b->getMass());

        // Calculate change in angular velocity of each body due to the impulse
        a->addAngularVelocity(-a->getInverseMomentOfInertia() * cross(r1, impulse));
        b->addAngularVelocity(b->getInverseMomentOfInertia() * cross(r2, impulse));

        const float slop = 0.01f; // 0.01f
        const float percent = 0.2f; // 0.2f
        vec2 correction = std::max(m->depth - slop, 0.0f) / (sumInvMass) * percent * m->normal;
        a->getEntity()->addPosition(-correction * aInvMass);
        b->getEntity()->addPosition(correction * bInvMass);

    }

}

World::World(float timeStep, vec2 gravity) {
    this->gravity = new Gravity(gravity);
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
    if (this->collisions.size() > 0) {std::cout << "RESOLUTION\n";}
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
                DebugDraw::drawBox(position, box->getSize(), box->getRotation(), vec3(0.0f, 1.0f, 0.0f), 1);
                //DebugDraw::drawBox(position, box->getSize(), 0.0f, vec3(0.0f, 1.0f, 0.0f), 1);
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