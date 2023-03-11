#include <math.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include "physics/collision/collision.hpp"
#include "physics/primitives/circle.hpp"
#include "physics/primitives/box.hpp"

#include <iostream>

namespace {

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

    CollisionManifold* findCollisionFeaturesBoxAndBox(Box* a, Box* b) {

        vec2 aMin = a->getMin();
        vec2 bMin = b->getMin();
        vec2 aMax = a->getMax();
        vec2 bMax = b->getMax();

        bool colliding = aMax.x > bMin.x && bMax.x > aMin.x && aMax.y > bMin.y && bMax.y > aMin.y;
        if (!colliding) {return NULL;}

        float xDepths[2];
        float yDepths[2];

        xDepths[0] = fabsf(bMax.x - aMin.x);
        xDepths[1] = fabsf(bMin.x - aMax.x);
        yDepths[0] = fabsf(bMax.y - aMin.y);
        yDepths[1] = fabsf(bMin.y - aMax.y);

        float minXDepth = std::min(xDepths[0], xDepths[1]);
        float minYDepth = std::min(yDepths[0], yDepths[1]);

        vec2 aPosition = a->getPosition();
        vec2 bPosition = b->getPosition();

        float depth;
        vec2 normal;
        vec2 contactPoint;

        if (minXDepth < minYDepth) {

            // Determine the normal of collision
            normal.x = (bPosition.x > aPosition.x) ? 1.0f : -1.0f;
            normal.y = 0.0f;

            // Find the middle of the contact point
            float yValues[4];
            float aY[2];
            float bY[2];
            aY[0] = aMin.y;
            aY[1] = aMax.y;
            bY[0] = bMin.y;
            bY[1] = bMax.y;

            int k = 0;
            int i = 0;
            int j = 0;

            // Combine two sorted arrays
            while (i < 2 && j < 2) {
                
                if (i >= 2) {
                    yValues[k] = bY[j];
                    j++;
                }

                else if (j >= 2) {
                    yValues[k] = aY[i];
                    i++;
                }

                else if (aY[i] < bY[j]) {
                    yValues[k] = aY[i];
                    i++;
                }

                else {
                    yValues[k] = bY[j];
                    j++;
                }

                k++;
            }

            float yMid = (yValues[2] - yValues[1]) * 0.5f;
            if (xDepths[0] < xDepths[1]) {contactPoint.x = aMin.x + normal.x * depth;} 
            else {contactPoint.x = aMax.x + normal.x * depth;}
            contactPoint.y = yMid;

        }

        else {

            // Determine the normal of collision
            normal.x = 0.0f;
            normal.y = (bPosition.y > aPosition.y) ? 1.0f : -1.0f;

            // Find the middle of the contact point
            float xValues[4];
            float aX[2];
            float bX[2];
            aX[0] = aMin.x;
            aX[1] = aMax.x;
            bX[0] = bMin.x;
            bX[1] = bMax.x;

            int k = 0;
            int i = 0;
            int j = 0;

            // Combine two sorted arrays
            while (i < 2 && j < 2) {
                
                if (i >= 2) {
                    xValues[k] = bX[j];
                    j++;
                }

                else if (j >= 2) {
                    xValues[k] = aX[i];
                    i++;
                }

                else if (aX[i] < bX[j]) {
                    xValues[k] = aX[i];
                    i++;
                }

                else {
                    xValues[k] = bX[j];
                    j++;
                }

                k++;
            }

            float xMid = (xValues[2] - xValues[1]) * 0.5f;
            if (yDepths[0] < yDepths[1]) {contactPoint.y = aMin.y + normal.y * depth;} 
            else {contactPoint.y = aMax.y + normal.y * depth;}
            contactPoint.x = xMid;

        }

        return new CollisionManifold(normal, contactPoint, depth);
    }

    CollisionManifold* findCollisionFeaturesCircleAndBox(Circle* c, Box* b, bool flip) {

        vec2 cPos = c->getPosition();
        float cRadius = c->getRadius();

        vec2 bPos = b->getPosition();
        vec2 bMin = b->getMin();
        vec2 bMax = b->getMax();

        float depth;
        vec2 normal;
        vec2 contactPoint;

        // Colliding with the top face of the box.
        if (cPos.x >= bMin.x && cPos.x <= bMax.x && cPos.y >= bMax.y && cPos.y < bMax.y + cRadius) {

            depth = (bMax.y - (cPos.y - cRadius)) * 0.5f;
            normal.x = 0.0f;
            normal.y = -1.0f;
            contactPoint.x = cPos.x;
            contactPoint.y = bMax.y - depth;
            if (flip) {normal = -normal;}

            return new CollisionManifold(normal, contactPoint, depth);
        }

        // Colliding with the bottom face of the box.
        if (cPos.x >= bMin.x && cPos.x <= bMax.x && cPos.y > bMin.y - cRadius && cPos.y <= bMin.y) {

            depth = ((cPos.y + cRadius) - bMin.y) * 0.5f;
            normal.x = 0.0f;
            normal.y = 1.0f;
            contactPoint.x = cPos.x;
            contactPoint.y = bMin.y + depth;
            if (flip) {normal = -normal;}

            return new CollisionManifold(normal, contactPoint, depth);
        }

        // Colliding with the right face of the box
        if (cPos.y >= bMin.y && cPos.y <= bMax.y && cPos.x >= bMax.x && cPos.x < bMax.x + cRadius) {

            depth = (bMax.x - (cPos.x - cRadius)) * 0.5f;
            normal.x = -1.0f;
            normal.y = 0.0f;
            contactPoint.x = cPos.y;
            contactPoint.y = bMax.x - depth;
            if (flip) {normal = -normal;}

            return new CollisionManifold(normal, contactPoint, depth);
        }

        // Colliding with the left face of the box.
        if (cPos.y >= bMin.y && cPos.y <= bMax.y && cPos.x > bMin.x - cRadius && cPos.x <= bMin.x) {

            depth = ((cPos.x + cRadius) - bMin.x) * 0.5f;
            normal.x = 1.0f;
            normal.y = 0.0f;
            contactPoint.x = cPos.y;
            contactPoint.y = bMin.x + depth;
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