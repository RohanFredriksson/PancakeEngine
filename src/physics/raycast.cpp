#include <cmath>
#include <limits>
#include <algorithm>
#include <cfloat>
#include "pancake/physics/raycast.hpp"
#include "pancake/physics/collider.hpp"

namespace Pancake {

    namespace {

        void rotateVector(vec2& vec, vec2 origin, float rCos, float rSin) {
            float x = vec.x - origin.x;
            float y = vec.y - origin.y;
            vec.x = origin.x + ((x * rCos) - (y * rSin));
            vec.y = origin.y + ((x * rSin) + (y * rCos));
        }

    }

    Ray::Ray(vec2 origin, vec2 direction) {
        this->origin = origin;
        this->direction = glm::normalize(direction);
    }

    void Ray::rotate(vec2 around, float rCos, float rSin) {
        rotateVector(this->origin, around, rCos, rSin);
        rotateVector(this->direction, vec2(0.0f, 0.0f), rCos, rSin);
    }

    RaycastResult::RaycastResult(vec2 point, vec2 normal, float distance, Entity* hit) {
        this->point = point;
        this->normal = normal;
        this->distance = distance;
        this->hit = hit;
    }

    RaycastResult::RaycastResult() {
        this->point = glm::vec2(0.0f, 0.0f);
        this->normal = glm::vec2(0.0f, 0.0f);
        this->distance = FLT_MAX;
        this->hit = nullptr;
    }

    namespace {

        RaycastResult raycastBoxCollider(BoxCollider* box, Ray ray) {

            // Transform the ray into the box's local space, where the box is axis aligned and centred at the origin.
            vec2 centre = box->getPosition();
            float radians = box->getRotation();
            float rCos = cosf(radians);
            float rSin = sinf(radians);
            vec2 origin = ray.origin;
            vec2 direction = ray.direction;
            rotateVector(origin, centre, rCos, -rSin);
            rotateVector(direction, vec2(0.0f, 0.0f), rCos, -rSin);
            origin -= centre;
            vec2 half = box->getSize() * 0.5f;

            // Slab test against both axes.
            float tEnter = -FLT_MAX;
            float tExit = FLT_MAX;
            vec2 enterNormal = vec2(0.0f, 0.0f);
            vec2 exitNormal = vec2(0.0f, 0.0f);

            for (int axis = 0; axis < 2; axis++) {

                if (std::abs(direction[axis]) < 1e-8f) {
                    if (std::abs(origin[axis]) > half[axis]) {return RaycastResult();}
                    continue;
                }

                float t1 = (-half[axis] - origin[axis]) / direction[axis];
                float t2 = ( half[axis] - origin[axis]) / direction[axis];
                float sign = direction[axis] > 0.0f ? 1.0f : -1.0f;

                // t1 is always the face the ray enters through on this axis.
                if (t1 > t2) {std::swap(t1, t2);}
                if (t1 > tEnter) {tEnter = t1; enterNormal = vec2(0.0f, 0.0f); enterNormal[axis] = -sign;}
                if (t2 < tExit) {tExit = t2; exitNormal = vec2(0.0f, 0.0f); exitNormal[axis] = sign;}

            }

            if (tEnter > tExit || tExit < 0.0f) {return RaycastResult();}

            // If the ray starts inside the box, report where it leaves.
            float t = tEnter;
            vec2 normal = enterNormal;
            if (tEnter < 0.0f) {t = tExit; normal = exitNormal;}

            // Rotate the normal back into global space.
            rotateVector(normal, vec2(0.0f, 0.0f), rCos, rSin);
            vec2 point = ray.origin + ray.direction * t;
            return RaycastResult(point, normal, t, box->getRigidbody()->getEntity());

        }

        RaycastResult raycastCircleCollider(CircleCollider* circle, Ray ray) {

            vec2 position = circle->getPosition();
            vec2 originToCircleCollider = position - ray.origin;
            float radiusSquared = circle->getRadius() * circle->getRadius();
            float originToCircleColliderLengthSquared = glm::dot(originToCircleCollider, originToCircleCollider);

            // Project the vector from the ray origin onto the direction of the ray.
            float a = glm::dot(originToCircleCollider, ray.direction);
            float b2 = originToCircleColliderLengthSquared - (a * a);
            if (radiusSquared - b2 < 0.0f) {return RaycastResult();}

            // If the ray starts inside the circle, report where it leaves.
            float f = sqrtf(radiusSquared - b2);
            float t = 0;
            if (originToCircleColliderLengthSquared < radiusSquared) {t = a + f;}
            else {t = a - f;}

            // The circle is behind the ray.
            if (t < 0.0f) {return RaycastResult();}

            vec2 point = ray.direction * t + ray.origin;
            vec2 normal = glm::normalize(point - position);
            return RaycastResult(point, normal, t, circle->getRigidbody()->getEntity());

        }

    }

    namespace Raycast {

        RaycastResult raycast(Rigidbody* rigidbody, Ray ray) {

            RaycastResult best;
            for (Collider* collider : rigidbody->getColliders()) {
                RaycastResult current;
                if (dynamic_cast<BoxCollider*>(collider) != nullptr) {current = raycastBoxCollider((BoxCollider*) collider, ray);}
                if (dynamic_cast<CircleCollider*>(collider) != nullptr) {current = raycastCircleCollider((CircleCollider*) collider, ray);}
                if (current.hit != nullptr && current.distance < best.distance) {best = current;}
            }

            return best;
        }

    }

}