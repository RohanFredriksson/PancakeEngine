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

        const int VELOCITY_ITERATIONS = 10;
        const int MAX_STEPS_PER_UPDATE = 5;
        const float RESTITUTION_THRESHOLD = 1.0f; // Contacts slower than this do not bounce, which stops resting bodies jittering.

        inline float cross(glm::vec2 a, glm::vec2 b) {
            return a.x * b.y - a.y * b.x;
        }

        // The velocity of a point on a body rotating with angular velocity w, offset r from its centroid.
        inline glm::vec2 cross(float w, glm::vec2 r) {
            return glm::vec2(-w * r.y, w * r.x);
        }

        // A single contact point between two bodies, with the impulses accumulated over the solver iterations.
        struct Contact {

            Rigidbody* a;
            Rigidbody* b;
            glm::vec2 normal;
            glm::vec2 tangent;
            glm::vec2 rA;
            glm::vec2 rB;
            float invMassA;
            float invMassB;
            float invMomentA;
            float invMomentB;
            float normalMass;
            float tangentMass;
            float friction;
            float bounce;
            float normalImpulse;
            float tangentImpulse;

            Contact(Rigidbody* a, Rigidbody* b, const CollisionManifold& m) {

                this->a = a;
                this->b = b;
                this->normal = m.normal;
                this->tangent = glm::vec2(-m.normal.y, m.normal.x);
                this->rA = m.point - a->getCentroid();
                this->rB = m.point - b->getCentroid();
                this->invMassA = a->getInverseMass();
                this->invMassB = b->getInverseMass();
                this->invMomentA = a->hasFixedOrientation() ? 0.0f : a->getInverseMomentOfInertia();
                this->invMomentB = b->hasFixedOrientation() ? 0.0f : b->getInverseMomentOfInertia();

                float rnA = cross(this->rA, this->normal);
                float rnB = cross(this->rB, this->normal);
                float kNormal = this->invMassA + this->invMassB + this->invMomentA * rnA * rnA + this->invMomentB * rnB * rnB;
                this->normalMass = kNormal > 0.0f ? 1.0f / kNormal : 0.0f;

                float rtA = cross(this->rA, this->tangent);
                float rtB = cross(this->rB, this->tangent);
                float kTangent = this->invMassA + this->invMassB + this->invMomentA * rtA * rtA + this->invMomentB * rtB * rtB;
                this->tangentMass = kTangent > 0.0f ? 1.0f / kTangent : 0.0f;

                this->friction = std::max(a->getFriction(), b->getFriction());
                float restitution = a->getRestitution() * b->getRestitution();
                float approach = glm::dot(this->relativeVelocity(), this->normal);
                this->bounce = approach < -RESTITUTION_THRESHOLD ? -restitution * approach : 0.0f;

                this->normalImpulse = 0.0f;
                this->tangentImpulse = 0.0f;

            }

            glm::vec2 relativeVelocity() {
                glm::vec2 vA = this->a->getVelocity() + cross(this->a->getAngularVelocity(), this->rA);
                glm::vec2 vB = this->b->getVelocity() + cross(this->b->getAngularVelocity(), this->rB);
                return vB - vA;
            }

            void apply(glm::vec2 impulse) {
                this->a->addVelocity(-impulse * this->invMassA);
                this->a->addAngularVelocity(-this->invMomentA * cross(this->rA, impulse));
                this->b->addVelocity(impulse * this->invMassB);
                this->b->addAngularVelocity(this->invMomentB * cross(this->rB, impulse));
            }

            // Start from the impulses applied at this contact last step, if it persisted.
            void warmStart(const std::vector<CachedContact>& cache, glm::vec2 point) {
                for (const CachedContact& c : cache) {
                    if (glm::dot(c.normal, this->normal) < 0.95f) {continue;}
                    if (glm::distance(c.point, point) > 0.05f) {continue;}
                    this->normalImpulse = c.normalImpulse;
                    this->tangentImpulse = std::clamp(c.tangentImpulse, -this->friction * this->normalImpulse, this->friction * this->normalImpulse);
                    this->apply(this->normalImpulse * this->normal + this->tangentImpulse * this->tangent);
                    return;
                }
            }

            void solve() {

                // Friction, limited by the normal impulse applied so far.
                float vt = glm::dot(this->relativeVelocity(), this->tangent);
                float limit = this->friction * this->normalImpulse;
                float tangentImpulse = std::clamp(this->tangentImpulse - vt * this->tangentMass, -limit, limit);
                float dt = tangentImpulse - this->tangentImpulse;
                this->tangentImpulse = tangentImpulse;
                this->apply(dt * this->tangent);

                // Normal impulse. The accumulated impulse may only push the bodies apart.
                float vn = glm::dot(this->relativeVelocity(), this->normal);
                float normalImpulse = std::max(this->normalImpulse + (this->bounce - vn) * this->normalMass, 0.0f);
                float dn = normalImpulse - this->normalImpulse;
                this->normalImpulse = normalImpulse;
                this->apply(dn * this->normal);

            }

        };

        // Push overlapping bodies apart, in proportion to their inverse masses, to stop them sinking into each other.
        void correctPositions(Rigidbody* a, Rigidbody* b, const std::vector<CollisionManifold>& manifolds) {

            const float slop = 0.005f;
            const float percent = 0.8f;

            float invMassA = a->getInverseMass();
            float invMassB = b->getInverseMass();
            if (invMassA + invMassB <= 0.0f) {return;}

            // Use the deepest contact, so two contacts along the same face don't double the correction.
            glm::vec2 correction = glm::vec2(0.0f, 0.0f);
            float deepest = 0.0f;
            for (const CollisionManifold& m : manifolds) {
                if (m.depth <= deepest) {continue;}
                deepest = m.depth;
                correction = m.normal * (std::max(m.depth - slop, 0.0f) * percent / (invMassA + invMassB));
            }

            a->getEntity()->addPosition(-correction * invMassA);
            b->getEntity()->addPosition(correction * invMassB);

        }

    }

    float World::getTimeStep() {
        return this->timeStep;
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

        // Apply force generators, then integrate forces and gravity into velocities.
        this->registry.updateForces(this->timeStep);
        for (Rigidbody* rigidbody : this->rigidbodies) {rigidbody->integrateVelocity(this->timeStep);}

        // Required data structures to store collision data.
        std::unordered_set<std::pair<int, int>, IntPairHash> colliding;
        std::vector<std::pair<Rigidbody*, Rigidbody*>> collisions;
        std::vector<std::vector<CollisionManifold>> manifolds;

        for (const std::vector<Rigidbody*>& bodies : *this->grid) {
            
            for (int i = 0; i < bodies.size(); i++) {

                for (int j = i + 1; j < bodies.size(); j++) {

                    // Get the next pairing, ordered by id so results don't depend on the grid's iteration order.
                    Rigidbody* rigidbody1 = bodies[i];
                    Rigidbody* rigidbody2 = bodies[j];
                    if (rigidbody2->getId() < rigidbody1->getId()) {std::swap(rigidbody1, rigidbody2);}

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
        
        // Resolve collisions with sequential impulses, accumulating and clamping the impulse at each contact.
        // Solve pairs in id order, so the result does not depend on memory layout.
        std::vector<int> order(collisions.size());
        for (int i = 0; i < order.size(); i++) {order[i] = i;}
        std::sort(order.begin(), order.end(), [&collisions](int x, int y) {
            std::pair<int, int> a = std::make_pair(collisions[x].first->getId(), collisions[x].second->getId());
            std::pair<int, int> b = std::make_pair(collisions[y].first->getId(), collisions[y].second->getId());
            return a < b;
        });

        // Contacts that persist from the last step start with the impulses they ended with.
        std::vector<Contact> contacts;
        std::vector<std::pair<int, int>> keys;
        std::vector<glm::vec2> points;
        for (int i : order) {
            std::pair<int, int> key = std::make_pair(collisions[i].first->getId(), collisions[i].second->getId());
            auto cached = this->contactCache.find(key);
            for (const CollisionManifold& m : manifolds[i]) {
                contacts.push_back(Contact(collisions[i].first, collisions[i].second, m));
                if (cached != this->contactCache.end()) {contacts.back().warmStart(cached->second, m.point);}
                keys.push_back(key);
                points.push_back(m.point);
            }
        }

        for (int k = 0; k < VELOCITY_ITERATIONS; k++) {
            for (Contact& contact : contacts) {contact.solve();}
        }

        // Remember the impulses for the next step.
        this->contactCache.clear();
        for (int i = 0; i < contacts.size(); i++) {
            this->contactCache[keys[i]].push_back({points[i], contacts[i].normal, contacts[i].normalImpulse, contacts[i].tangentImpulse});
        }

        // Move the bodies with their resolved velocities, then push apart any that still overlap.
        for (Rigidbody* rigidbody : this->rigidbodies) {rigidbody->integratePosition(this->timeStep);}
        for (int i : order) {
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