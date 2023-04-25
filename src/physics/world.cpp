#include <cmath>
#include <limits>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "pancake/physics/world.hpp"
#include "pancake/graphics/debugdraw.hpp"

namespace Pancake {

    namespace {

        class ImpulseResult {
            
            public:

                vec2 aVelocity;
                vec2 bVelocity;
                float aAngularVelocity;
                float bAngularVelocity;
                float aAngularMagnitude;
                float bAngularMagnitude;
                float aAngularVelocityLost;
                float bAngularVelocityLost;

                ImpulseResult(vec2 aVelocity, vec2 bVelocity, float aAngularVelocity, float bAngularVelocity) {
                    this->aVelocity = aVelocity;
                    this->bVelocity = bVelocity;
                    this->aAngularVelocity = aAngularVelocity;
                    this->bAngularVelocity = bAngularVelocity;
                    this->aAngularVelocityLost = 0.0f;
                    this->bAngularVelocityLost = 0.0f;
                }

                ImpulseResult operator*(float scalar) const {
                    return ImpulseResult(this->aVelocity * scalar, this->bVelocity * scalar, this->aAngularVelocity * scalar, this->bAngularVelocity * scalar);
                }

                ImpulseResult& operator+=(const ImpulseResult& rhs) {
                    this->aVelocity += rhs.aVelocity;
                    this->bVelocity += rhs.bVelocity;
                    if ((this->aAngularVelocity > 0 && rhs.aAngularVelocity < 0) || (this->aAngularVelocity < 0 && rhs.aAngularVelocity > 0)) {this->aAngularVelocityLost += fabsf(rhs.aAngularVelocity);}
                    if ((this->bAngularVelocity > 0 && rhs.bAngularVelocity < 0) || (this->bAngularVelocity < 0 && rhs.bAngularVelocity > 0)) {this->bAngularVelocityLost += fabsf(rhs.bAngularVelocity);}
                    this->aAngularVelocity += rhs.aAngularVelocity;
                    this->bAngularVelocity += rhs.bAngularVelocity;
                    return *this;
                }

        };
        
        const int IMPULSE_ITERATIONS = 6;
        
        inline vec2 perp(vec2 v) {
            return glm::rotate(v, glm::half_pi<float>());
        }

        inline float proj(vec2 a, vec2 b) {
            return glm::dot(a, b) / glm::length(b);
        }

        ImpulseResult applyImpulse(Rigidbody* a, Rigidbody* b, CollisionManifold* m) {

            // Check for infinite mass objects.
            ImpulseResult result(vec2(0.0f, 0.0f), vec2(0.0f, 0.0f), 0.0f, 0.0f);
            if (a->hasInfiniteMass() && b->hasInfiniteMass()) {return result;}

            DebugDraw::drawBox(m->contactPoint, vec2(0.1f, 0.1f), 0.0f, vec3(1.0f, 0.0f, 0.0f), 10);
            DebugDraw::drawLine(m->contactPoint, m->contactPoint + m->normal * 0.3f, vec3(0.0f, 0.0f, 1.0f), 10);

            float invMassA = a->getInverseMass();
            float invMassB = b->getInverseMass();
            float invMoiA = a->getInverseMomentOfInertia();
            float invMoiB = b->getInverseMomentOfInertia();
            float angVelA = a->getAngularVelocity();
            float angVelB = b->getAngularVelocity();
            float restitution = a->getRestitution() * b->getRestitution();
            float friction = std::max(a->getFriction(), b->getFriction());
            vec2 tangent = perp(m->normal);
            vec2 rA = m->contactPoint - a->getCentroid();
            vec2 rB = m->contactPoint - b->getCentroid();
            vec2 vAB = b->getVelocity() + perp(rB) * angVelB - a->getVelocity() - perp(rA) * angVelA;
            float rAproj = proj(rA, m->normal);
            float rAreg = proj(rA, tangent);
            float rBproj = proj(rB, m->normal);
            float rBreg = proj(rB, tangent);
            float vproj = proj(vAB, m->normal);
            float vreg = proj(vAB, tangent);

            // If both bodies are moving away from each other.
            if (vproj >= 0.0f) {return result;}

            // Calculate the impulse of the collision.
            float impulse =  -((1 + restitution) * vproj) / (invMassA + invMassB + (invMoiA * rAreg * rAreg) + (invMoiB * rBreg * rBreg));

            // Resolve the collision.
            result.aVelocity += -m->normal * impulse * invMassA;
            result.bVelocity += m->normal * impulse * invMassB;
            result.aAngularVelocity += impulse * invMoiA * rAreg;
            result.bAngularVelocity += -impulse * invMoiB * rBreg;

            // Calculate friction.
            if (friction > 0.0f) {

                float max = fabsf(vreg) / (invMassA + invMassB + (invMoiA * rAproj * rAproj) + (invMoiB * rBproj * rBproj));
                float sign = vreg < 0.0f ? -1.0f : 1.0f;
                impulse = friction * fabsf(impulse);
                impulse = std::min(impulse, max);
                impulse *= sign;

                result.aVelocity += tangent * impulse * invMassA;
                result.bVelocity += -tangent * impulse * invMassB;
                result.aAngularVelocity += impulse * invMoiA * rAproj;
                result.bAngularVelocity += -impulse * invMoiB * rBproj;

            }

            // Position correction.
            if (!a->hasInfiniteMass() && !b->hasInfiniteMass()) {
                const float slop = 0.01f;
                const float percent = 0.2f;
                vec2 correction = std::max(m->depth - slop, 0.0f) / (invMassA + invMassB) * percent * m->normal;
                a->getEntity()->addPosition(-correction * invMassA);
                b->getEntity()->addPosition(correction * invMassB);
            }
            
            if (b->hasInfiniteMass()) {a->getEntity()->addPosition(1.01f * -m->depth * m->normal);}
            if (a->hasInfiniteMass()) {b->getEntity()->addPosition(1.01f *  m->depth * m->normal);}

            return result;
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

                vector<CollisionManifold*> results;
                for (int k = 0; k < colliders1.size(); k++) {
                    for (int l = 0; l < colliders2.size(); l++) {
                        
                        Collider* collider1 = colliders1[k];
                        Collider* collider2 = colliders2[l];

                        CollisionManifold* result = Collision::findCollisionFeatures(collider1, collider2);
                        if (result != nullptr) {results.push_back(result);}

                    }
                }

                if (results.size() > 0) {

                    //rigidbody1->getEntity()->onCollision(rigidbody2);
                    //rigidbody2->getEntity()->onCollision(rigidbody1);

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
                ImpulseResult sum(vec2(0.0f, 0.0f), vec2(0.0f, 0.0f), 0.0f, 0.0f);

                float depthTotal = 0.0f;
                for (int j = 0; j < this->collisions[i].size(); j++) {depthTotal += this->collisions[i][j]->depth;}
                for (int j = 0; j < this->collisions[i].size(); j++) {sum += applyImpulse(r1, r2, this->collisions[i][j]) * (this->collisions[i][j]->depth / depthTotal);}

                r1->addVelocity(sum.aVelocity);
                r2->addVelocity(sum.bVelocity);
                if (sum.aAngularVelocityLost > 0.0f && glm::dot(sum.aVelocity, sum.aVelocity) > 0.0f) {r1->addVelocity(glm::normalize(sum.aVelocity) * sum.aAngularVelocityLost);}
                if (sum.bAngularVelocityLost > 0.0f && glm::dot(sum.bVelocity, sum.bVelocity) > 0.0f) {r2->addVelocity(glm::normalize(sum.bVelocity) * sum.bAngularVelocityLost);}
                r1->addAngularVelocity(sum.aAngularVelocity);
                r2->addAngularVelocity(sum.bAngularVelocity);

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
                }

                else if (dynamic_cast<Circle*>(collider) != nullptr) {
                    Circle* circle = (Circle*) collider;
                    DebugDraw::drawCircle(position, circle->getRadius(), vec3(0.0f, 1.0f, 0.0f), 1);
                }

            }

        }

    }

    json World::serialise() {
        json j;
        j.emplace("gravity", json::array());
        j["gravity"].push_back(this->getGravity().x);
        j["gravity"].push_back(this->getGravity().y);
        return j;
    }

    void World::load(json j) {
        if (j.contains("gravity") && j["gravity"].is_array() && j["gravity"].size() == 2 && j["gravity"][0].is_number() && j["gravity"][1].is_number()) {
            this->setGravity(vec2(j["gravity"][0], j["gravity"][1]));
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
            if (current.hit != nullptr && current.distance < bestDistance) {
                bestDistance = current.distance;
                best = current;
                hit = true;
            }
        }

        if (hit) {return RaycastResult();}
        return best;

    }

    vec2 World::getGravity() {
        return this->gravity->getGravity();
    }

    void World::setGravity(vec2 gravity) {
        this->gravity->setGravity(gravity);
    }

}