#include <cmath>
#include <algorithm>
#include "pancake/physics/raycast/raycast.hpp"
#include "pancake/physics/primitives/collider.hpp"
#include "pancake/physics/primitives/box.hpp"
#include "pancake/physics/primitives/circle.hpp"

Ray::Ray(vec2 origin, vec2 direction) {
    this->origin = origin;
    this->direction = glm::normalize(direction);
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
    this->distance = 0.0f;
    this->hit = nullptr;
}

namespace {

    RaycastResult raycastBox(Box* box, Ray* ray) {

        vec2 unitVector = glm::normalize(ray->direction);
        vec2 min = box->getMin();
        vec2 max = box->getMax();

        float tMin = std::max(std::min(min.x, max.x), std::min(min.y, max.y));
        float tMax = std::min(std::max(min.x, max.x), std::max(min.y, max.y));

        if (tMax < 0 || tMin > tMax) {return RaycastResult();}

        float t = (tMin < 0.0f) ? tMax : tMin;
        bool hit = t > 0.0f;
        if (!hit) {return RaycastResult();}

        vec2 point = ray->direction * t + ray->origin;
        vec2 normal = glm::normalize(ray->origin - point);
        return RaycastResult(point, normal, t, box->getRigidbody()->getEntity());

    }

    RaycastResult raycastCircle(Circle* circle, Ray* ray) {

        vec2 position = circle->getPosition();
        vec2 originToCircle = position - ray->origin;
        float radiusSquared = circle->getRadius() * circle->getRadius();
        float originToCircleLengthSquared = glm::dot(originToCircle, originToCircle);

        // Project the vector from the ray origin onto the direction of the ray.
        float a = glm::dot(originToCircle, ray->direction);
        float b2 = originToCircleLengthSquared - (a * a);
        if (radiusSquared - b2 < 0.0f) {return RaycastResult();}

        float f = sqrtf(radiusSquared - b2);
        float t = 0;
        if (originToCircleLengthSquared < radiusSquared) {t = a + f;}
        else {t = a - f;}

        vec2 point = ray->direction * t + ray->origin;
        vec2 normal = glm::normalize(ray->origin - point);
        return RaycastResult(point, normal, t, circle->getRigidbody()->getEntity());

    }

}

namespace Raycast {

    RaycastResult raycast(Rigidbody* rigidbody, Ray* ray) {

        //Collider* collider = rigidbody->getCollider();
        //if (collider == nullptr) {return RaycastResult();}

        //if (dynamic_cast<Box*>(collider) != nullptr) {
        //    return raycastBox((Box*) collider, ray);
        //}

        //else if (dynamic_cast<Circle*>(collider) != nullptr) {
        //    return raycastCircle((Circle*) collider, ray);
        //}

        return RaycastResult();
    }

}

