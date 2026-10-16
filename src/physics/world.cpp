#include <cmath>
#include <limits>
#include <algorithm>
#include <cfloat>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "pancake/core/factory.hpp"
#include "pancake/physics/world.hpp"
#include "pancake/graphics/debugdraw.hpp"

namespace Pancake {

    namespace {

        class ImpulseResult {
            
            public:

                glm::vec2 aVelocity;
                glm::vec2 bVelocity;
                float aAngularVelocity;
                float bAngularVelocity;
                float aAngularVelocityTotal;
                float bAngularVelocityTotal;
                float aAngularOrbitalSpeed;
                float bAngularOrbitalSpeed;

                ImpulseResult() {
                    this->aVelocity = glm::vec2(0.0f, 0.0f);
                    this->bVelocity = glm::vec2(0.0f, 0.0f);
                    this->aAngularVelocity = 0.0f;
                    this->bAngularVelocity = 0.0f;
                    this->aAngularVelocityTotal = 0.0f;
                    this->bAngularVelocityTotal = 0.0f;
                    this->aAngularOrbitalSpeed = 0.0f;
                    this->bAngularOrbitalSpeed = 0.0f;
                }

                ImpulseResult operator*(float scalar) const {
                    ImpulseResult result;
                    result.aVelocity = this->aVelocity * scalar;
                    result.bVelocity = this->bVelocity * scalar;
                    result.aAngularVelocity = this->aAngularVelocity * scalar;
                    result.bAngularVelocity = this->bAngularVelocity * scalar;
                    result.aAngularVelocityTotal = this->aAngularVelocityTotal * scalar;
                    result.bAngularVelocityTotal = this->bAngularVelocityTotal * scalar;
                    result.aAngularOrbitalSpeed = this->aAngularOrbitalSpeed * scalar;
                    result.bAngularOrbitalSpeed = this->bAngularOrbitalSpeed * scalar;
                    return result;                    
                }

                ImpulseResult& operator+=(const ImpulseResult& rhs) {
                    this->aVelocity += rhs.aVelocity;
                    this->bVelocity += rhs.bVelocity;
                    this->aAngularVelocity += rhs.aAngularVelocity;
                    this->bAngularVelocity += rhs.bAngularVelocity;
                    this->aAngularVelocityTotal += rhs.aAngularVelocityTotal;
                    this->bAngularVelocityTotal += rhs.bAngularVelocityTotal;
                    this->aAngularOrbitalSpeed += rhs.aAngularOrbitalSpeed;
                    this->bAngularOrbitalSpeed += rhs.bAngularOrbitalSpeed;
                    return *this;
                }

        };
        
        const int IMPULSE_ITERATIONS = 6;
        const int MAX_STEPS_PER_UPDATE = 5;
        
        inline glm::vec2 perp(glm::vec2 v) {
            return glm::rotate(v, glm::half_pi<float>());
        }

        inline float proj(glm::vec2 a, glm::vec2 b) {
            return glm::dot(a, b) / glm::length(b);
        }

        ImpulseResult applyImpulse(Rigidbody* a, Rigidbody* b, CollisionManifold m) {

            // Check for infinite mass objects.
            ImpulseResult result;
            if (a->hasInfiniteMass() && b->hasInfiniteMass()) {return result;}

            float invMassA = a->getInverseMass();
            float invMassB = b->getInverseMass();
            float invMoiA = a->getInverseMomentOfInertia();
            float invMoiB = b->getInverseMomentOfInertia();
            float angVelA = a->getAngularVelocity();
            float angVelB = b->getAngularVelocity();
            float restitution = a->getRestitution() * b->getRestitution();
            float friction = std::max(a->getFriction(), b->getFriction());
            glm::vec2 tangent = perp(m.normal);
            glm::vec2 rA = m.point - a->getCentroid();
            glm::vec2 rB = m.point - b->getCentroid();
            glm::vec2 vAB = b->getVelocity() + perp(rB) * angVelB - a->getVelocity() - perp(rA) * angVelA;
            float rAproj = proj(rA, m.normal);
            float rAreg = proj(rA, tangent);
            float rBproj = proj(rB, m.normal);
            float rBreg = proj(rB, tangent);
            float vproj = proj(vAB, m.normal);
            float vreg = proj(vAB, tangent);

            // If both bodies are moving away from each other.
            if (vproj >= 0.0f) {return result;}

            // Calculate the impulse of the collision.
            float impulse =  -((1 + restitution) * vproj) / (invMassA + invMassB + (invMoiA * rAreg * rAreg) + (invMoiB * rBreg * rBreg));

            // Resolve the collision.
            result.aVelocity -= m.normal * impulse * invMassA;
            result.bVelocity += m.normal * impulse * invMassB;
            result.aAngularVelocity += impulse * invMoiA * rAreg;
            result.bAngularVelocity -= impulse * invMoiB * rBreg;
            result.aAngularVelocityTotal += fabsf(impulse * invMoiA * rAreg);
            result.bAngularVelocityTotal += fabsf(impulse * invMoiB * rBreg);
            result.aAngularOrbitalSpeed += fabsf(impulse * invMoiA * rAreg * rAreg);
            result.bAngularOrbitalSpeed += fabsf(impulse * invMoiB * rBreg * rBreg);

            // Calculate friction.
            if (friction > 0.0f) {

                float max = fabsf(vreg) / (invMassA + invMassB + (invMoiA * rAproj * rAproj) + (invMoiB * rBproj * rBproj));
                float sign = vreg < 0.0f ? -1.0f : 1.0f;
                impulse = friction * fabsf(impulse);
                impulse = std::min(impulse, max);
                impulse *= sign;

                result.aVelocity += tangent * impulse * invMassA;
                result.bVelocity -= tangent * impulse * invMassB;
                result.aAngularVelocity += impulse * invMoiA * rAproj;
                result.bAngularVelocity -= impulse * invMoiB * rBproj;
                result.aAngularVelocityTotal += fabsf(impulse * invMoiA * rAproj);
                result.bAngularVelocityTotal += fabsf(impulse * invMoiB * rBproj);

            }

            return result;
        }

        // Push overlapping bodies apart, in proportion to their inverse masses, to stop them sinking into each other.
        void correctPositions(Rigidbody* a, Rigidbody* b, const std::vector<CollisionManifold>& manifolds) {

            const float slop = 0.005f;
            const float percent = 0.8f;

            float invMassA = a->getInverseMass();
            float invMassB = b->getInverseMass();
            if (invMassA + invMassB <= 0.0f) {return;}

            // Weight each contact by its depth, as with the impulses.
            float depthTotal = 0.0f;
            for (const CollisionManifold& m : manifolds) {depthTotal += m.depth;}
            if (depthTotal <= 0.0f) {return;}

            glm::vec2 correction = glm::vec2(0.0f, 0.0f);
            for (const CollisionManifold& m : manifolds) {
                float magnitude = std::max(m.depth - slop, 0.0f) * percent / (invMassA + invMassB);
                correction += m.normal * magnitude * (m.depth / depthTotal);
            }

            a->getEntity()->addPosition(-correction * invMassA);
            b->getEntity()->addPosition(correction * invMassB);

        }

    }

    World::World(float timeStep, glm::vec2 gravity) {
        this->grid = new SpatialHashGrid<Rigidbody*>(4);
        this->gravity = gravity;
        this->timeStep = timeStep;
        this->time = 0.0f;
        this->debug = false;
    }

    World::~World() {

        // Rigidbodies and force generators are components, so they are owned and deleted by their entities.
        // Detach any that are still registered so they don't reference this world after it is gone.
        for (Rigidbody* rigidbody : this->rigidbodies) {rigidbody->setWorld(nullptr);}
        for (ForceGenerator* force : this->forces) {force->setWorld(nullptr);}
        delete this->grid;

    }

    void World::update(float dt) {

        // Step at a fixed rate. If we fall too far behind, drop the excess time rather than spiralling.
        this->time += dt;
        int steps = 0;
        while (this->time >= this->timeStep && steps < MAX_STEPS_PER_UPDATE) {
            this->fixedUpdate();
            this->time -= this->timeStep;
            steps++;
        }
        if (steps == MAX_STEPS_PER_UPDATE) {this->time = 0.0f;}

    }

    void World::fixedUpdate() {

        // Update velocities of all rigidbodies
        int n = this->rigidbodies.size();
        for (int i = 0; i < n; i++) {
            this->rigidbodies[i]->physicsUpdate(this->timeStep);
        }

        // Required data structures to store collision data.
        std::unordered_set<std::pair<int, int>, IntPairHash> colliding;
        std::vector<std::pair<Rigidbody*, Rigidbody*>> collisions;
        std::vector<std::vector<CollisionManifold>> manifolds;

        for (const std::vector<Rigidbody*>& bodies : *this->grid) {
            
            for (int i = 0; i < bodies.size(); i++) {

                for (int j = i + 1; j < bodies.size(); j++) {

                    // Get the next pairing
                    Rigidbody* rigidbody1 = bodies[i];
                    Rigidbody* rigidbody2 = bodies[j];

                    // Check if this pairing has already been collided.
                    int smaller = std::min(rigidbody1->getId(), rigidbody2->getId());
                    int larger = std::max(rigidbody1->getId(), rigidbody2->getId());
                    std::pair<int, int> ids = std::make_pair(smaller, larger);
                    auto it = colliding.find(ids);
                    if (it != colliding.end()) {continue;}

                    // Check if they have infinite mass.
                    if (rigidbody1->hasInfiniteMass() && rigidbody2->hasInfiniteMass()) {continue;}

                    // Get the colliders of each body.
                    std::vector<Collider*> colliders1 = rigidbody1->getColliders();
                    std::vector<Collider*> colliders2 = rigidbody2->getColliders();

                    // If either has no colliders then we cant collide.
                    if (colliders1.size() == 0 || colliders2.size() == 0) {continue;}

                    // Test collision of each pairing of colliders.
                    std::vector<CollisionManifold> results;
                    for (int k = 0; k < colliders1.size(); k++) {
                        for (int l = 0; l < colliders2.size(); l++) {
                            
                            Collider* collider1 = colliders1[k];
                            Collider* collider2 = colliders2[l];

                            std::vector<CollisionManifold> features = Collision::findCollisionFeatures(collider1, collider2);
                            for (CollisionManifold feature : features) {results.push_back(feature);}

                        }
                    }

                    // If there is a new collision that we havent found yet.
                    if (results.size() > 0) {
                        
                        // Add the ID pair so we dont collide them again.
                        colliding.insert(ids);

                        // Update the listeners
                        for (CollisionManifold manifold : results) {

                            for (Component* component : rigidbody1->getEntity()->getComponents()) {
                                CollisionListener* listener = dynamic_cast<CollisionListener*>(component);
                                if (listener != nullptr) {listener->collision(rigidbody2->getEntity(), manifold);}
                            }

                            CollisionManifold flipped = manifold.flip();
                            for (Component* component : rigidbody2->getEntity()->getComponents()) {
                                CollisionListener* listener = dynamic_cast<CollisionListener*>(component);
                                if (listener != nullptr) {listener->collision(rigidbody1->getEntity(), flipped);}
                            }

                        }

                        // Visualise the contacts.
                        if (this->debug) {
                            for (CollisionManifold manifold : results) {
                                DebugDraw::drawBox(manifold.point, glm::vec2(0.1f, 0.1f), 0.0f, vec3(1.0f, 0.0f, 0.0f), 10);
                                DebugDraw::drawLine(manifold.point, manifold.point + manifold.normal * 0.3f, vec3(0.0f, 0.0f, 1.0f), 10);
                            }
                        }

                        // Add the bodies to the impulse list if they are not sensors.
                        if (!rigidbody1->isSensor() && !rigidbody2->isSensor()) {
                            collisions.push_back(std::make_pair(rigidbody1, rigidbody2));
                            manifolds.push_back(results);
                        }

                    }

                }

            }

        }
        
        // Update the forces
        this->registry.updateForces(this->timeStep);

        // Resolve collision via iterative impulse resolution.
        for (int k = 0; k < IMPULSE_ITERATIONS; k++) {
            int m = collisions.size();
            for (int i = 0; i < m; i++) {

                Rigidbody* r1 = collisions[i].first;
                Rigidbody* r2 = collisions[i].second;
                ImpulseResult sum;

                // Weight each contact by its depth, or equally if they are only touching.
                int count = manifolds[i].size();
                float depthTotal = 0.0f;
                for (int j = 0; j < count; j++) {depthTotal += manifolds[i][j].depth;}
                for (int j = 0; j < count; j++) {
                    float weight = depthTotal > 0.0f ? manifolds[i][j].depth / depthTotal : 1.0f / count;
                    sum += applyImpulse(r1, r2, manifolds[i][j]) * weight;
                }

                r1->addVelocity(sum.aVelocity);
                r2->addVelocity(sum.bVelocity);
                r1->addAngularVelocity(sum.aAngularVelocity);
                r2->addAngularVelocity(sum.bAngularVelocity);

                if (sum.aAngularVelocityTotal > 0.0f && glm::dot(sum.aVelocity, sum.aVelocity) > 0.0f) {r1->addVelocity(((sum.aAngularVelocityTotal - fabsf(sum.aAngularVelocity)) / sum.aAngularVelocityTotal) * sum.aAngularOrbitalSpeed * glm::normalize(sum.aVelocity));}
                if (sum.bAngularVelocityTotal > 0.0f && glm::dot(sum.bVelocity, sum.bVelocity) > 0.0f) {r2->addVelocity(((sum.bAngularVelocityTotal - fabsf(sum.bAngularVelocity)) / sum.bAngularVelocityTotal) * sum.bAngularOrbitalSpeed * glm::normalize(sum.bVelocity));}

            }
            
        }

        // Correct positions once, after the velocities have been resolved.
        for (int i = 0; i < collisions.size(); i++) {
            correctPositions(collisions[i].first, collisions[i].second, manifolds[i]);
        }

    }

    void World::render() {

        if (!this->debug) {return;}

        // Draw the bounding box and colliders of every rigidbody.
        for (Rigidbody* rigidbody : this->rigidbodies) {

            std::pair<glm::vec2, glm::vec2> bounds = rigidbody->getBounds();
            DebugDraw::drawAABB(bounds.first, bounds.second, vec3(0.5f, 0.0f, 1.0f), 1);

            for (Collider* collider : rigidbody->getColliders()) {
                BoxCollider* box = dynamic_cast<BoxCollider*>(collider);
                CircleCollider* circle = dynamic_cast<CircleCollider*>(collider);
                if (box != nullptr) {DebugDraw::drawBox(box->getPosition(), box->getSize(), box->getRotation(), vec3(0.0f, 1.0f, 0.0f), 1);}
                if (circle != nullptr) {DebugDraw::drawCircleCollider(circle->getPosition(), circle->getRadius(), vec3(0.0f, 1.0f, 0.0f), 1);}
            }

        }

    }

    glm::vec2 World::getGravity() {
        return this->gravity;
    }

    void World::setGravity(glm::vec2 gravity) {
        this->gravity = gravity;
    }

    bool World::hasDebugDraw() {
        return this->debug;
    }

    void World::setDebugDraw(bool debug) {
        this->debug = debug;
    }

    void World::add(Rigidbody* rigidbody) {

        // A rigidbody can only belong to one world at a time.
        if (this->has(rigidbody)) {return;}
        if (rigidbody->getWorld() != nullptr) {rigidbody->getWorld()->remove(rigidbody);}
        rigidbody->setWorld(this);
        
        // Insert the rigidbody in the index and the vector.
        this->rigidbodies.push_back(rigidbody);
        this->rigidbodiesIndex.insert(rigidbody);

        // Add the rigidbody to the spatial hash grid.
        std::pair<glm::vec2, glm::vec2> bounds = rigidbody->getBounds();
        this->grid->add(rigidbody, bounds.first, bounds.second);

        // Apply all required force generators to the rigidbody.
        std::unordered_set<std::string> generators = rigidbody->getForceGenerators();
        for (const std::string& type : generators) {
            this->addForceRegistration(type, rigidbody);
        }

    }

    bool World::has(Rigidbody* rigidbody) {
        auto search = this->rigidbodiesIndex.find(rigidbody);
        return search != this->rigidbodiesIndex.end();
    }

    void World::remove(Rigidbody* rigidbody) {

        // Remove the rigidbody from the index and the spatial hash grid.
        if (!this->has(rigidbody)) {return;}
        rigidbody->setWorld(nullptr);
        this->rigidbodiesIndex.erase(rigidbody);
        this->grid->remove(rigidbody);

        // Remove the rigidbody from the rigidbody vector.
        int n = this->rigidbodies.size();
        for (int i = 0; i < n; i++) {
            
            if (this->rigidbodies[i] == rigidbody) {

                std::unordered_set<std::string> generators = rigidbody->getForceGenerators();
                for (const std::string& type : generators) {
                    this->removeForceRegistration(type, rigidbody);
                }

                this->rigidbodies.erase(this->rigidbodies.begin() + i);
                return;

            }
        
        }

    }

    RaycastResult World::raycast(Ray ray) {

        // An unbounded ray could cross any number of cells, so test every body.
        RaycastResult best;
        for (Rigidbody* rigidbody : this->rigidbodies) {
            RaycastResult current = Raycast::raycast(rigidbody, ray);
            if (current.hit != nullptr && current.distance < best.distance) {best = current;}
        }

        return best;
    }

    RaycastResult World::raycast(Ray ray, float maxDistance) {

        if (!std::isfinite(maxDistance)) {return this->raycast(ray);}

        // Walk the spatial hash grid cells along the ray (Amanatides & Woo), testing the bodies in each cell.
        float size = (float) this->grid->getGridSize();
        int x = (int) std::floor(ray.origin.x / size);
        int y = (int) std::floor(ray.origin.y / size);
        int stepX = ray.direction.x > 0.0f ? 1 : -1;
        int stepY = ray.direction.y > 0.0f ? 1 : -1;

        float nextX = (x + (stepX > 0 ? 1 : 0)) * size;
        float nextY = (y + (stepY > 0 ? 1 : 0)) * size;
        float tMaxX = ray.direction.x != 0.0f ? (nextX - ray.origin.x) / ray.direction.x : FLT_MAX;
        float tMaxY = ray.direction.y != 0.0f ? (nextY - ray.origin.y) / ray.direction.y : FLT_MAX;
        float tDeltaX = ray.direction.x != 0.0f ? size / std::abs(ray.direction.x) : FLT_MAX;
        float tDeltaY = ray.direction.y != 0.0f ? size / std::abs(ray.direction.y) : FLT_MAX;

        RaycastResult best;
        std::unordered_set<Rigidbody*> tested;

        while (true) {

            for (Rigidbody* rigidbody : this->grid->get(x, y)) {
                if (!tested.insert(rigidbody).second) {continue;}
                RaycastResult current = Raycast::raycast(rigidbody, ray);
                if (current.hit != nullptr && current.distance <= maxDistance && current.distance < best.distance) {best = current;}
            }

            // A hit closer than the far edge of this cell cannot be beaten by bodies in later cells.
            float tCellExit = std::min(tMaxX, tMaxY);
            if (best.hit != nullptr && best.distance <= tCellExit) {break;}
            if (tCellExit > maxDistance) {break;}

            if (tMaxX < tMaxY) {x += stepX; tMaxX += tDeltaX;}
            else {y += stepY; tMaxY += tDeltaY;}

        }

        return best;
    }


    SpatialHashGrid<Rigidbody*>* World::getGrid() {
        return this->grid;
    }

    ForceGenerator* World::getForceGenerator(std::string type) {
        auto it = this->forcesIndex.find(type);
        if (it != this->forcesIndex.end()) {return it->second;}
        return nullptr;
    }

    void World::addForceGenerator(ForceGenerator* force) {

        // Remove the current force if they share the same name.
        std::string type = force->getType();
        auto it = this->forcesIndex.find(type);
        if (it != this->forcesIndex.end()) {this->removeForceGenerator(type);}

        // Add the force to the index.
        this->forcesIndex.insert({type, force});
        this->forces.push_back(force);
        force->setWorld(this);

        // See if any rigidbodies require this force generator.
        for (Rigidbody* rigidbody : this->rigidbodies) {
            if (rigidbody->hasForceGenerator(type)) {this->addForceRegistration(type, rigidbody);}
        }

    }

    void World::removeForceGenerator(ForceGenerator* force) {
        // Only remove the generator if it is the one currently registered under its type.
        if (this->getForceGenerator(force->getType()) != force) {return;}
        this->removeForceGenerator(force->getType());
    }

    void World::removeForceGenerator(std::string type) {
        
        // See if the force exists in the index.
        auto it = this->forcesIndex.find(type);
        if (it == this->forcesIndex.end()) {return;}

        // Remove all registrations that use this force.
        ForceGenerator* force = it->second;
        this->registry.remove(force);
        if (force->getWorld() == this) {force->setWorld(nullptr);}

        // Remove the force from the index.
        this->forcesIndex.erase(type);

        // Remove the force from the vector.
        for (int i = 0; i < this->forces.size(); i++) {
            if (this->forces[i]->getType() == type) {
                this->forces.erase(this->forces.begin() + i);
                return;
            }
        }

    }

    void World::addForceRegistration(std::string force, Rigidbody* rigidbody) {
        this->removeForceRegistration(force, rigidbody);
        ForceGenerator* generator = this->getForceGenerator(force);
        if (generator == nullptr) {return;}
        this->registry.add(generator, rigidbody);
    }

    void World::removeForceRegistration(std::string force, Rigidbody* rigidbody) {
        ForceGenerator* generator = this->getForceGenerator(force);
        if (generator == nullptr) {return;}
        this->registry.remove(generator, rigidbody);
    }

}