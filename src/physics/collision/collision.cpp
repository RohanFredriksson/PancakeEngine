#include <cmath>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include "physics/collision/collision.hpp"
#include "physics/primitives/circle.hpp"
#include "physics/primitives/box.hpp"

using std::vector;

namespace {

    bool xSort(const glm::vec2& a, const glm::vec2& b) {
        return a.x < b.x;
    }

    bool ySort(const glm::vec2& a, const glm::vec2& b) {
        return a.y < b.y;
    }

    void rotate(vec2& vec, vec2 origin, float rCos, float rSin) {
        float x = vec.x - origin.x;
        float y = vec.y - origin.y;
        vec.x = origin.x + ((x * rCos) - (y * rSin));
        vec.y = origin.y + ((x * rSin) + (y * rCos));
    }

    void rotate(vector<vec2>& vertices, vec2 origin, float rCos, float rSin) {
        int n = vertices.size();
        for (int i = 0; i < n; i++) {
            float x = vertices[i].x - origin.x;
            float y = vertices[i].y - origin.y;
            vertices[i].x = origin.x + ((x * rCos) - (y * rSin));
            vertices[i].x = origin.y + ((x * rSin) + (y * rCos));
        }
    }

    bool intersects(vec2 aStart, vec2 aEnd, vec2 bStart, vec2 bEnd) {
        float dx0 = aEnd.x - aStart.x;
        float dx1 = bEnd.x - bStart.x;
        float dy0 = aEnd.y - aStart.y;
        float dy1 = bEnd.y - bStart.y;
        float p0 = dy1 * (bEnd.x - aStart.x) - dx1 *(bEnd.y - aStart.y);
        float p1 = dy1 * (bEnd.x - aEnd.x)   - dx1 *(bEnd.y - aEnd.y);
        float p2 = dy0 * (aEnd.x - bStart.x) - dx0 *(aEnd.y - bStart.y);
        float p3 = dy0 * (aEnd.x - bEnd.x)   - dx0 *(aEnd.y - bEnd.y);
        return (p0 * p1 <= 0) && (p2 * p3 <= 0);
    }

    CollisionManifold* findCollisionFeaturesCircleAndCircle(Circle* a, Circle* b) {

        vec2 aPosition = a->getPosition();
        vec2 bPosition = b->getPosition();

        // Determine if the two circles are colliding.
        float sumRadii = a->getRadius() + b->getRadius();
        vec2 distance = bPosition - aPosition;
        if (glm::dot(distance, distance) - (sumRadii * sumRadii) > 0) {return NULL;}

        // Find the depth and normal of the collision
        float depth = fabsf(glm::length(distance) - sumRadii) * 0.5f;
        vec2 normal = glm::normalize(distance);

        // Find the contact point of the collision
        float distanceToPoint = a->getRadius() - depth;
        vec2 contactPoint = distanceToPoint * normal + aPosition;

        return new CollisionManifold(normal, contactPoint, depth);
    }

    CollisionManifold* findCollisionFeaturesAABBAndBox(vec2 aMin, vec2 aMax, vector<vec2> bVertices, bool flip) {
        
        // Find all the vertices in b that are inside a's aabb.
        vector<vec2> inside;
        vector<vec2> outside;
        for (vec2 v : bVertices) {
            if (v.x > aMin.x && v.x < aMax.x && v.y > aMin.y && v.y < aMax.y) {inside.push_back(v);}
            else {outside.push_back(v);}
        }

        // Find the size that minimises the straight line distance of all points to an edge.
        if (inside.size() == 1 || inside.size() == 2) {

        } 

        // Find the edge the outside vertex is.
        if (inside.size() == 3) {

        }

        // Either no collision or all inside the shape.
        return NULL;
    }

    CollisionManifold* findCollisionFeaturesBoxAndBox(Box* a, Box* b) {

        // Store all the information that is required.
        float aRotation = a->getRotation();
        vec2 aPos = a->getPosition();
        vec2 aMin = a->getMin();
        vec2 aMax = a->getMax();

        float bRotation = b->getRotation();
        vec2 bPos = b->getPosition();
        vec2 bMin = b->getMin();
        vec2 bMax = b->getMax();

        // Rotate the vertices of b, such that we can treat a as an aabb.
        vector<vec2> vertices;
        vertices.push_back(vec2(bMin.x, bMin.y));
        vertices.push_back(vec2(bMin.x, bMax.y));
        vertices.push_back(vec2(bMax.x, bMax.y));
        vertices.push_back(vec2(bMin.x, bMax.y));

        float rCos = cosf(-aRotation);
        float rSin = sinf(-aRotation);
        rotate(vertices, aPos, rCos, rSin);

        // Check for collisions where a is an aabb.
        CollisionManifold* result = findCollisionFeaturesAABBAndBox(aMin, aMax, vertices, false);
        if (result != NULL) {return result;}

        // Rotate the vertices of a, such that we can treat a as an aabb.
        vertices.clear();
        vertices.push_back(vec2(aMin.x, aMin.y));
        vertices.push_back(vec2(aMin.x, aMax.y));
        vertices.push_back(vec2(aMax.x, aMax.y));
        vertices.push_back(vec2(aMin.x, aMax.y));

        rotate(vertices, bPos, rCos, -rSin);

        // Check for collision where b is an aabb.
        return findCollisionFeaturesAABBAndBox(bMin, bMax, vertices, true);

    }

    CollisionManifold* findCollisionFeaturesCircleAndBox(Circle* c, Box* b, bool flip) {

        // Get the circle's properties.
        vec2 cPos = c->getPosition();
        float cRadius = c->getRadius();

        // Get the box's properties.
        float bRot = b->getRotation();
        vec2 bPos = b->getPosition();
        vec2 bMin = b->getMin();
        vec2 bMax = b->getMax();

        // If we need to rotate, we need to store these variables.
        vec2 offset;
        float rCos;
        float rSin;
        
        // Rotate the circle into the box's space.
        if (bRot != 0.0f) {
            rCos = cosf(-bRot);
            rSin = sinf(-bRot);
            rotate(cPos, bPos, rCos, rSin);
        }

        float depth;
        vec2 normal;
        vec2 contactPoint;

        // Colliding with the top face of the box.
        if (cPos.x >= bMin.x && cPos.x <= bMax.x && cPos.y >= bMax.y && cPos.y < bMax.y + cRadius) {

            depth = (bMax.y - (cPos.y - cRadius)) * 0.5f;
            normal = vec2(0.0f, -1.0f);
            contactPoint = vec2(cPos.x, bMax.y - depth);
            if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(contactPoint, bPos, rCos, -rSin);}
            if (flip) {normal = -normal;}

            return new CollisionManifold(normal, contactPoint, depth);
        }

        // Colliding with the bottom face of the box.
        if (cPos.x >= bMin.x && cPos.x <= bMax.x && cPos.y > bMin.y - cRadius && cPos.y <= bMin.y) {

            depth = ((cPos.y + cRadius) - bMin.y) * 0.5f;
            normal = vec2(0.0f, 1.0f);
            contactPoint = vec2(cPos.x, bMin.y + depth);
            if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(contactPoint, bPos, rCos, -rSin);}
            if (flip) {normal = -normal;}

            return new CollisionManifold(normal, contactPoint, depth);
        }

        // Colliding with the right face of the box
        if (cPos.y >= bMin.y && cPos.y <= bMax.y && cPos.x >= bMax.x && cPos.x < bMax.x + cRadius) {

            depth = (bMax.x - (cPos.x - cRadius)) * 0.5f;
            normal = vec2(-1.0f, 0.0f);
            contactPoint = vec2(bMax.x - depth, cPos.y);
            if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(contactPoint, bPos, rCos, -rSin);}
            if (flip) {normal = -normal;}

            return new CollisionManifold(normal, contactPoint, depth);
        }

        // Colliding with the left face of the box.
        if (cPos.y >= bMin.y && cPos.y <= bMax.y && cPos.x > bMin.x - cRadius && cPos.x <= bMin.x) {

            depth = ((cPos.x + cRadius) - bMin.x) * 0.5f;
            normal = vec2(1.0f, 0.0f);
            contactPoint = vec2(bMin.x + depth, cPos.y);
            if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(contactPoint, bPos, rCos, -rSin);}
            if (flip) {normal = -normal;}

            return new CollisionManifold(normal, contactPoint, depth);
        }

        // Colliding with the top left corner of the box.
        if (glm::dot(vec2(cPos.x - bMin.x, cPos.y - bMax.y), vec2(cPos.x - bMin.x, cPos.y - bMax.y)) < cRadius * cRadius) {

            vec2 difference = vec2(bMin.x, bMax.y) - cPos;
            normal = glm::normalize(difference);
            vec2 depthVector = ((normal * cRadius) - difference) * 0.5f;            
            depth = glm::length(depthVector);
            contactPoint = vec2(bMin.x, bMax.y) + depthVector;
            if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(contactPoint, bPos, rCos, -rSin);}
            if (flip) {normal = -normal;}

            return new CollisionManifold(normal, contactPoint, depth);
        }

        // Colliding with the top right corner of the box.
        if (glm::dot(vec2(cPos.x - bMax.x, cPos.y - bMax.y), vec2(cPos.x - bMax.x, cPos.y - bMax.y)) < cRadius * cRadius) {

            vec2 difference = vec2(bMax.x, bMax.y) - cPos;
            normal = glm::normalize(difference);
            vec2 depthVector = ((normal * cRadius) - difference) * 0.5f;            
            depth = glm::length(depthVector);
            contactPoint = vec2(bMax.x, bMax.y) + depthVector;
            if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(contactPoint, bPos, rCos, -rSin);}
            if (flip) {normal = -normal;}

            return new CollisionManifold(normal, contactPoint, depth);
        }

        // Colliding with the bottom left corner of the box.
        if (glm::dot(vec2(cPos.x - bMin.x, cPos.y - bMin.y), vec2(cPos.x - bMin.x, cPos.y - bMin.y)) < cRadius * cRadius) {

            vec2 difference = vec2(bMin.x, bMin.y) - cPos;
            normal = glm::normalize(difference);
            vec2 depthVector = ((normal * cRadius) - difference) * 0.5f;            
            depth = glm::length(depthVector);
            contactPoint = vec2(bMin.x, bMin.y) + depthVector;
            if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(contactPoint, bPos, rCos, -rSin);}
            if (flip) {normal = -normal;}

            return new CollisionManifold(normal, contactPoint, depth);
        }

        // Colliding with the bottom right corner of the box.
        if (glm::dot(vec2(cPos.x - bMax.x, cPos.y - bMin.y), vec2(cPos.x - bMax.x, cPos.y - bMin.y)) < cRadius * cRadius) {

            vec2 difference = vec2(bMax.x, bMin.y) - cPos;
            normal = glm::normalize(difference);
            vec2 depthVector = ((normal * cRadius) - difference) * 0.5f;            
            depth = glm::length(depthVector);
            contactPoint = vec2(bMax.x, bMin.y) + depthVector;
            if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(contactPoint, bPos, rCos, -rSin);}
            if (flip) {normal = -normal;}

            return new CollisionManifold(normal, contactPoint, depth);
        }
        return NULL;
    }

}

namespace Collision {
    
    CollisionManifold* findCollisionFeatures(Collider* c1, Collider* c2) {

        if (dynamic_cast<Box*>(c1) != nullptr && dynamic_cast<Box*>(c2) != nullptr) {return findCollisionFeaturesBoxAndBox((Box*) c1, (Box*) c2);}
        if (dynamic_cast<Circle*>(c1) != nullptr && dynamic_cast<Circle*>(c2) != nullptr) {return findCollisionFeaturesCircleAndCircle((Circle*) c1, (Circle*) c2);}
        if (dynamic_cast<Circle*>(c1) != nullptr && dynamic_cast<Box*>(c2) != nullptr) {return findCollisionFeaturesCircleAndBox((Circle*) c1, (Box*) c2, false);}
        if (dynamic_cast<Box*>(c1) != nullptr && dynamic_cast<Circle*>(c2) != nullptr) {return findCollisionFeaturesCircleAndBox((Circle*) c2, (Box*) c1, true);}

        return NULL;
    }

}