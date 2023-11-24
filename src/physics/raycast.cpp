#include <cmath>
#include <limits>
#include <algorithm>
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

            // Rotate the ray into the box's local space.
            vec2 centre = box->getPosition();
            float radians = box->getRotation();
            float rCos = cosf(radians);
            float rSin = sinf(radians);
            ray.rotate(centre, rCos, rSin);

            // Perform the ray cast in local space.
            vec2 unitVector = glm::normalize(ray.direction);
            vec2 min = box->getPosition() - box->getSize() * 0.5f;
            vec2 max = box->getPosition() + box->getSize() * 0.5f;

            float tMin = std::max(std::min(min.x, max.x), std::min(min.y, max.y));
            float tMax = std::min(std::max(min.x, max.x), std::max(min.y, max.y));

            if (tMax < 0 || tMin > tMax) {return RaycastResult();}

            float t = (tMin < 0.0f) ? tMax : tMin;
            bool hit = t > 0.0f;
            if (!hit) {return RaycastResult();}

            // Get the point and normal in the box's local space.
            vec2 point = ray.direction * t + ray.origin;
            vec2 normal = glm::normalize(ray.origin - point);

            // Rotate the result back into global space.
            rotateVector(point, centre, rCos, -rSin);
            rotateVector(normal, vec2(0.0f, 0.0f), rCos, -rSin);

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

            float f = sqrtf(radiusSquared - b2);
            float t = 0;
            if (originToCircleColliderLengthSquared < radiusSquared) {t = a + f;}
            else {t = a - f;}

            vec2 point = ray.direction * t + ray.origin;
            vec2 normal = glm::normalize(ray.origin - point);
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