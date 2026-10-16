#include <cmath>
#include <cfloat>
#include <limits>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include "pancake/physics/force.hpp"
#include "pancake/physics/collider.hpp"
#include "pancake/physics/collision.hpp"

using std::vector;

namespace Pancake {

    CollisionManifold::CollisionManifold() {
        this->normal = glm::vec2(0.0f, 0.0f);
        this->point = glm::vec2(0.0f, 0.0f);
        this->depth = 0.0f;
    }

    CollisionManifold::CollisionManifold(vec2 normal, vec2 point, float depth) {
        this->normal = normal;
        this->point = point;
        this->depth = depth;
    }

    CollisionManifold CollisionManifold::flip() {
        return CollisionManifold(this->normal * -1.0f, this->point, this->depth);
    }

    namespace {

        // All features use the same conventions: the normal points from the first collider towards the second,
        // the depth is the full penetration depth, and the point lies midway between the two surfaces.

        vec2 rotate(vec2 v, float rCos, float rSin) {
            return vec2(v.x * rCos - v.y * rSin, v.x * rSin + v.y * rCos);
        }

        struct OrientedBox {
            vec2 centre;
            vec2 axes[2];      // Local x and y axes in world space.
            vec2 half;         // Half extents along each axis.
            vec2 vertices[4];  // Counter clockwise, starting bottom left.
            vec2 normals[4];   // Outward face normals, normals[i] belongs to the edge vertices[i] -> vertices[i + 1].
        };

        OrientedBox orient(BoxCollider* box) {
            OrientedBox o;
            float r = box->getRotation();
            float c = cosf(r);
            float s = sinf(r);
            o.centre = box->getPosition();
            o.axes[0] = vec2(c, s);
            o.axes[1] = vec2(-s, c);
            o.half = glm::abs(box->getSize()) * 0.5f;
            vec2 x = o.axes[0] * o.half.x;
            vec2 y = o.axes[1] * o.half.y;
            o.vertices[0] = o.centre - x - y;
            o.vertices[1] = o.centre + x - y;
            o.vertices[2] = o.centre + x + y;
            o.vertices[3] = o.centre - x + y;
            o.normals[0] = -o.axes[1];
            o.normals[1] =  o.axes[0];
            o.normals[2] =  o.axes[1];
            o.normals[3] = -o.axes[0];
            return o;
        }

        // The largest separation of b from any face of a, and which face it is.
        float findMaxSeparation(const OrientedBox& a, const OrientedBox& b, int& face) {
            float best = -FLT_MAX;
            for (int i = 0; i < 4; i++) {
                float separation = FLT_MAX;
                for (int j = 0; j < 4; j++) {separation = std::min(separation, glm::dot(a.normals[i], b.vertices[j] - a.vertices[i]));}
                if (separation > best) {best = separation; face = i;}
            }
            return best;
        }

        // Clip the segment to the side of the plane dot(normal, x) <= offset.
        int clipSegment(vec2 in[2], vec2 out[2], vec2 normal, float offset) {
            int count = 0;
            float d0 = glm::dot(normal, in[0]) - offset;
            float d1 = glm::dot(normal, in[1]) - offset;
            if (d0 <= 0.0f) {out[count++] = in[0];}
            if (d1 <= 0.0f) {out[count++] = in[1];}
            if (d0 * d1 < 0.0f) {out[count++] = in[0] + (d0 / (d0 - d1)) * (in[1] - in[0]);}
            return count;
        }

        std::vector<CollisionManifold> findCollisionFeaturesCircleColliderAndCircleCollider(CircleCollider* a, CircleCollider* b) {

            std::vector<CollisionManifold> result;

            vec2 difference = b->getPosition() - a->getPosition();
            float sumRadii = a->getRadius() + b->getRadius();
            float distanceSquared = glm::dot(difference, difference);
            if (distanceSquared >= sumRadii * sumRadii) {return result;}

            float distance = sqrtf(distanceSquared);
            vec2 normal = distance > 1e-6f ? difference / distance : vec2(0.0f, 1.0f);
            float depth = sumRadii - distance;
            vec2 point = a->getPosition() + normal * (a->getRadius() - 0.5f * depth);

            result.push_back(CollisionManifold(normal, point, depth));
            return result;

        }

        std::vector<CollisionManifold> findCollisionFeaturesBoxColliderAndBoxCollider(BoxCollider* boxA, BoxCollider* boxB) {

            std::vector<CollisionManifold> result;
            OrientedBox a = orient(boxA);
            OrientedBox b = orient(boxB);

            // Separating axis test on the face normals of both boxes.
            int faceA = 0;
            int faceB = 0;
            float separationA = findMaxSeparation(a, b, faceA);
            if (separationA > 0.0f) {return result;}
            float separationB = findMaxSeparation(b, a, faceB);
            if (separationB > 0.0f) {return result;}

            // Use the face with the least penetration as the reference face, preferring a for stability.
            const OrientedBox* reference = &a;
            const OrientedBox* incident = &b;
            int referenceFace = faceA;
            bool flipped = false;
            if (separationB > 0.98f * separationA + 0.001f) {
                reference = &b;
                incident = &a;
                referenceFace = faceB;
                flipped = true;
            }

            // The incident face is the one most anti-parallel to the reference face.
            vec2 referenceNormal = reference->normals[referenceFace];
            int incidentFace = 0;
            float minimum = FLT_MAX;
            for (int i = 0; i < 4; i++) {
                float d = glm::dot(referenceNormal, incident->normals[i]);
                if (d < minimum) {minimum = d; incidentFace = i;}
            }

            vec2 incidentEdge[2] = {incident->vertices[incidentFace], incident->vertices[(incidentFace + 1) % 4]};
            vec2 v1 = reference->vertices[referenceFace];
            vec2 v2 = reference->vertices[(referenceFace + 1) % 4];
            vec2 tangent = glm::normalize(v2 - v1);

            // Clip the incident edge to the sides of the reference face.
            vec2 clipped1[2];
            vec2 clipped2[2];
            if (clipSegment(incidentEdge, clipped1, -tangent, -glm::dot(tangent, v1)) < 2) {return result;}
            if (clipSegment(clipped1, clipped2, tangent, glm::dot(tangent, v2)) < 2) {return result;}

            // Keep the points below the reference face.
            vec2 normal = flipped ? -referenceNormal : referenceNormal;
            float offset = glm::dot(referenceNormal, v1);
            for (int i = 0; i < 2; i++) {
                float separation = glm::dot(referenceNormal, clipped2[i]) - offset;
                if (separation <= 0.0f) {
                    float depth = -separation;
                    result.push_back(CollisionManifold(normal, clipped2[i] + referenceNormal * (0.5f * depth), depth));
                }
            }

            return result;
        }

        std::vector<CollisionManifold> findCollisionFeaturesCircleColliderAndBoxCollider(CircleCollider* c, BoxCollider* b, bool flip) {

            std::vector<CollisionManifold> result;

            // Move the circle into the box's local space.
            vec2 bPos = b->getPosition();
            vec2 half = glm::abs(b->getSize()) * 0.5f;
            float rCos = cosf(b->getRotation());
            float rSin = sinf(b->getRotation());
            vec2 local = rotate(c->getPosition() - bPos, rCos, -rSin);
            float radius = c->getRadius();

            vec2 normal; // From the box towards the circle, in local space.
            vec2 surface;
            float depth;

            vec2 closest = glm::clamp(local, -half, half);
            if (closest == local) {

                // The centre is inside the box, so push it out through the nearest face.
                float dx = half.x - std::abs(local.x);
                float dy = half.y - std::abs(local.y);
                if (dx < dy) {
                    float sign = local.x < 0.0f ? -1.0f : 1.0f;
                    normal = vec2(sign, 0.0f);
                    surface = vec2(sign * half.x, local.y);
                    depth = radius + dx;
                } else {
                    float sign = local.y < 0.0f ? -1.0f : 1.0f;
                    normal = vec2(0.0f, sign);
                    surface = vec2(local.x, sign * half.y);
                    depth = radius + dy;
                }

            } else {

                vec2 difference = local - closest;
                float distanceSquared = glm::dot(difference, difference);
                if (distanceSquared >= radius * radius) {return result;}
                float distance = sqrtf(distanceSquared);
                normal = difference / distance;
                surface = closest;
                depth = radius - distance;

            }

            // Back into world space. The midpoint lies half the depth outward from the box surface.
            vec2 point = bPos + rotate(surface + normal * (0.5f * depth), rCos, rSin);
            normal = rotate(normal, rCos, rSin);

            // Unflipped, the circle is the first collider, so the normal must point from the circle to the box.
            if (!flip) {normal = -normal;}
            result.push_back(CollisionManifold(normal, point, depth));
            return result;
        }

    }

    namespace Collision {
        
        std::vector<CollisionManifold> findCollisionFeatures(Collider* c1, Collider* c2) {

            if (dynamic_cast<BoxCollider*>(c1) != nullptr && dynamic_cast<BoxCollider*>(c2) != nullptr) {return findCollisionFeaturesBoxColliderAndBoxCollider((BoxCollider*) c1, (BoxCollider*) c2);}
            if (dynamic_cast<CircleCollider*>(c1) != nullptr && dynamic_cast<CircleCollider*>(c2) != nullptr) {return findCollisionFeaturesCircleColliderAndCircleCollider((CircleCollider*) c1, (CircleCollider*) c2);}
            if (dynamic_cast<CircleCollider*>(c1) != nullptr && dynamic_cast<BoxCollider*>(c2) != nullptr) {return findCollisionFeaturesCircleColliderAndBoxCollider((CircleCollider*) c1, (BoxCollider*) c2, false);}
            if (dynamic_cast<BoxCollider*>(c1) != nullptr && dynamic_cast<CircleCollider*>(c2) != nullptr) {return findCollisionFeaturesCircleColliderAndBoxCollider((CircleCollider*) c2, (BoxCollider*) c1, true);}

            std::vector<CollisionManifold> empty;
            return empty;

        }

    }

}