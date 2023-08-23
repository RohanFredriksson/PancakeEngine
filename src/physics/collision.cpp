#include <cmath>
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
                vertices[i].y = origin.y + ((x * rSin) + (y * rCos));
            }
        }

        vec2 project(const vec2& v, const vec2& target) {
            vec2 targetUnit = glm::normalize(target);
            float projection = glm::dot(v, targetUnit);
            return projection * targetUnit;
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

        float distance(vec2 aStart, vec2 aEnd, vec2 bStart, vec2 bEnd) {

            // If either line is vertical, rotate the problem 45 degrees.
            // Since we are solving for distance, direction does not need to be preserved.
            if (aStart.x == aEnd.x || bStart.x == bEnd.x) {
                rotate(aStart, vec2(0.0f, 0.0f), 0.70710678118f, 0.70710678118f);
                rotate(aEnd,   vec2(0.0f, 0.0f), 0.70710678118f, 0.70710678118f);
                rotate(bStart, vec2(0.0f, 0.0f), 0.70710678118f, 0.70710678118f);
                rotate(bEnd,   vec2(0.0f, 0.0f), 0.70710678118f, 0.70710678118f);
            }

            // y = ax + b
            float a = (aEnd.y - aStart.y) / (aEnd.x - aStart.x);
            float b = aStart.y - a * aStart.x;
            
            // y = cx + d
            float c = (bEnd.y - bStart.y) / (bEnd.x - bStart.x);
            float d = bStart.y - c * bStart.x;

            // ax + b = cx + d: solve for x
            float x = (d - b) / (a - c);

            // plug x into ax + b
            float y = a * x + b;

            // Now that we have the point, compute the distance between points.
            vec2 point = vec2(x, y);
            return glm::distance(aStart, point);

        }

        float distance(vec2 start, vec2 end, vec2 point) {

            // If the line is vertical, rotate the problem 45 degrees.
            if (start.x == end.x) {
                rotate(start, vec2(0.0f, 0.0f), 0.70710678118f, 0.70710678118f);
                rotate(end,   vec2(0.0f, 0.0f), 0.70710678118f, 0.70710678118f);
                rotate(point, vec2(0.0f, 0.0f), 0.70710678118f, 0.70710678118f);
            }

            // y = ax + b
            float a = (end.y - start.y) / (end.x - start.x);
            float b = start.y - a * start.x;

            // Perpendicular distance formula
            float distance = fabsf(a * point.x + -1.0f * point.y + b) / glm::distance(start, end);
            return distance;

        }

        std::vector<CollisionManifold> findCollisionFeaturesCircleAndCircle(Circle* a, Circle* b) {

            std::vector<CollisionManifold> result;

            vec2 aPosition = a->getPosition();
            vec2 bPosition = b->getPosition();

            // Determine if the two circles are colliding.
            float sumRadii = a->getRadius() + b->getRadius();
            vec2 distance = bPosition - aPosition;
            if (glm::dot(distance, distance) - (sumRadii * sumRadii) > 0) {return result;}

            // Find the depth and normal of the collision
            float depth = fabsf(glm::length(distance) - sumRadii) * 0.5f;
            vec2 normal = glm::normalize(distance);

            // Find the contact point of the collision
            float distanceToPoint = a->getRadius() - depth;
            vec2 point = distanceToPoint * normal + aPosition;

            result.push_back(CollisionManifold(normal, point, depth));
            return result;

        }

        std::vector<CollisionManifold> findCollisionFeaturesBoxAndBox(Box* a, Box* b) {

            std::vector<CollisionManifold> result;

            // Store all the information that is required.
            float aRotation = a->getRotation();
            vec2 aSize = a->getSize();
            vec2 aPos = a->getPosition();
            vec2 aMin = a->getMin();
            vec2 aMax = a->getMax();

            float bRotation = b->getRotation();
            vec2 bSize = b->getSize();
            vec2 bPos = b->getPosition();
            vec2 bMin = b->getMin();
            vec2 bMax = b->getMax();

            // Cache the trig ratios for rotational use.
            float aCos = cosf(aRotation);
            float aSin = sinf(aRotation);
            float bCos = cosf(bRotation);
            float bSin = sinf(bRotation);

            // Get the vertices of a.
            vector<vec2> aVertices;
            aVertices.push_back(vec2(aMin.x, aMin.y)); // Bottom Left
            aVertices.push_back(vec2(aMin.x, aMax.y)); // Top Left
            aVertices.push_back(vec2(aMax.x, aMax.y)); // Top Right
            aVertices.push_back(vec2(aMax.x, aMin.y)); // Bottom Right
            rotate(aVertices, aPos, aCos, aSin);
            rotate(aVertices, bPos, bCos, -bSin);

            // Get the vertices of b.
            vector<vec2> bVertices;
            bVertices.push_back(vec2(bMin.x, bMin.y)); // Bottom Left
            bVertices.push_back(vec2(bMin.x, bMax.y)); // Top Left
            bVertices.push_back(vec2(bMax.x, bMax.y)); // Top Right
            bVertices.push_back(vec2(bMax.x, bMin.y)); // Bottom Right
            rotate(bVertices, bPos, bCos, bSin); // B Space -> Global
            rotate(bVertices, aPos, aCos, -aSin);// Global -> A Space

            // Get the positions of each box in their local spaces.
            vec2 aPosInB = aPos;
            vec2 bPosInA = bPos;
            rotate(aPosInB, bPos, bCos, -bSin);
            rotate(bPosInA, aPos, aCos, -aSin);

            // Determine which vertices of each box is in each others box.
            vector<vec2> bInsideA; for (vec2 vertex : bVertices) {if (vertex.x > aMin.x && vertex.x < aMax.x && vertex.y > aMin.y && vertex.y < aMax.y) {bInsideA.push_back(vertex);}}
            vector<vec2> aInsideB; for (vec2 vertex : aVertices) {if (vertex.x > bMin.x && vertex.x < bMax.x && vertex.y > bMin.y && vertex.y < bMax.y) {aInsideB.push_back(vertex);}}

            // Collision manifold variables.
            vec2 normal;
            vec2 point;
            float depth;

            // Special case for 1 and 1.
            if (aInsideB.size() == 1 && bInsideA.size() == 1) {
                
                // Find which side of a is b.
                vec2 other = aInsideB[0]; 
                rotate(other, bPos, bCos, bSin); 
                rotate(other, aPos, aCos, -aSin);                
                vec2 difference = 0.5f * (bInsideA[0] + other) - aPos;
                difference.x /= aSize.x;
                difference.y /= aSize.y;

                // Determine the normal direction.
                if (fabsf(difference.x) > fabsf(difference.y)) {if (difference.x >= 0.0f) {normal = vec2(1.0f, 0.0f);} else {normal = vec2(-1.0f, 0.0f);}}
                else                                           {if (difference.y >= 0.0f) {normal = vec2(0.0f, 1.0f);} else {normal = vec2( 0.0f,-1.0f);}}

                // Determine the depth.
                vec2 end = bInsideA[0] + std::max(std::max(aSize.x, aSize.y), std::max(bSize.x, bSize.y)) * normal;
                if      (intersects(bInsideA[0], end, vec2(aMin.x, aMin.y), vec2(aMin.x, aMax.y))) {depth = 0.5f * distance(bInsideA[0], end, vec2(aMin.x, aMin.y), vec2(aMin.x, aMax.y));}
                else if (intersects(bInsideA[0], end, vec2(aMin.x, aMin.y), vec2(aMax.x, aMin.y))) {depth = 0.5f * distance(bInsideA[0], end, vec2(aMin.x, aMin.y), vec2(aMax.x, aMin.y));}
                else if (intersects(bInsideA[0], end, vec2(aMin.x, aMax.y), vec2(aMax.x, aMax.y))) {depth = 0.5f * distance(bInsideA[0], end, vec2(aMin.x, aMax.y), vec2(aMax.x, aMax.y));}
                else                                                                               {depth = 0.5f * distance(bInsideA[0], end, vec2(aMax.x, aMin.y), vec2(aMax.x, aMax.y));}
                
                // Determine the contact point.
                point = bInsideA[0] + depth * normal;

                // Rotate into global space.
                rotate(normal, vec2(0.0f, 0.0f), aCos, aSin);
                rotate(point, aPos, aCos, aSin);

                // Add the point to the manifold.
                result.push_back(CollisionManifold(normal, point, depth * 0.5f));

                // Find which side of b is a.
                other = bInsideA[0];
                rotate(other, aPos, aCos, aSin); 
                rotate(other, bPos, bCos, -bSin);             
                difference = 0.5f * (aInsideB[0] + other) - bPos;
                difference.x /= bSize.x;
                difference.y /= bSize.y;

                // Determine the normal direction.
                if (fabsf(difference.x) > fabsf(difference.y)) {if (difference.x >= 0.0f) {normal = vec2(1.0f, 0.0f);} else {normal = vec2(-1.0f, 0.0f);}}
                else                                           {if (difference.y >= 0.0f) {normal = vec2(0.0f, 1.0f);} else {normal = vec2( 0.0f,-1.0f);}}

                // Determine the depth.
                end = aInsideB[0] + std::max(std::max(aSize.x, aSize.y), std::max(bSize.x, bSize.y)) * normal;
                if      (intersects(aInsideB[0], end, vec2(bMin.x, bMin.y), vec2(bMin.x, bMax.y))) {depth = 0.5f * distance(aInsideB[0], end, vec2(bMin.x, bMin.y), vec2(bMin.x, bMax.y));}
                else if (intersects(aInsideB[0], end, vec2(bMin.x, bMin.y), vec2(bMax.x, bMin.y))) {depth = 0.5f * distance(aInsideB[0], end, vec2(bMin.x, bMin.y), vec2(bMax.x, bMin.y));}
                else if (intersects(aInsideB[0], end, vec2(bMin.x, bMax.y), vec2(bMax.x, bMax.y))) {depth = 0.5f * distance(aInsideB[0], end, vec2(bMin.x, bMax.y), vec2(bMax.x, bMax.y));}
                else                                                                               {depth = 0.5f * distance(aInsideB[0], end, vec2(bMax.x, bMin.y), vec2(bMax.x, bMax.y));}
                
                // Determine the contact point.
                point = aInsideB[0] + depth * normal;

                // Rotate into global space.
                rotate(normal, vec2(0.0f, 0.0f), bCos, bSin);
                rotate(point, bPos, bCos, bSin);

                // Add the point to the manifold.
                result.push_back(CollisionManifold(normal * -1.0f, point, depth * 0.5f));
                return result;

            }

            // Flip the problem to reduce number of cases.
            bool flip = false;
            if (aInsideB.size() > bInsideA.size()) {
                flip = true; 
                vector<vec2> tmp = bInsideA;
                bInsideA = aInsideB; aInsideB = tmp; bVertices = aVertices;
                aMax = bMax; aMin = bMin; aPos = bPos; aCos = bCos; aSin = bSin; 
            }

            if (aInsideB.size() == 0 && bInsideA.size() == 1) {

                // Find the side that minimises the distance
                float best = FLT_MAX;
                float current;
                current = aMax.y - bInsideA[0].y; if (current < best) {normal = vec2( 0.0f,  1.0f); depth = current * 0.5f; point = bInsideA[0] + normal * depth; best = current;} // Top
                current = bInsideA[0].y - aMin.y; if (current < best) {normal = vec2( 0.0f, -1.0f); depth = current * 0.5f; point = bInsideA[0] + normal * depth; best = current;} // Bottom
                current = aMax.x - bInsideA[0].x; if (current < best) {normal = vec2( 1.0f,  0.0f); depth = current * 0.5f; point = bInsideA[0] + normal * depth; best = current;} // Right
                current = bInsideA[0].x - aMin.x; if (current < best) {normal = vec2(-1.0f,  0.0f); depth = current * 0.5f; point = bInsideA[0] + normal * depth; best = current;} // Left

                // Rotate back into global coordinates.
                rotate(normal, vec2(0.0f, 0.0f), aCos, aSin);
                rotate(point, aPos, aCos, aSin);
                if (flip) {normal = -normal;}

                result.push_back(CollisionManifold(normal, point, depth));
                return result;

            }

            if (aInsideB.size() == 0 && bInsideA.size() == 2) {
                
                // Collision manifold variables.
                vec2 points[2];
                float depths[2];

                // Find the side that minimises the distance.
                float best = FLT_MAX;
                float current;
                current = 0.0f; for (vec2 v : bInsideA) {current += aMax.y - v.y;} if (current < best) {normal = vec2( 0.0f,  1.0f); for (int i = 0; i < 2; i++) {depths[i] = (aMax.y - bInsideA[i].y) * 0.5f; points[i] = bInsideA[i] + normal * depths[i]; best = current;}} // Top
                current = 0.0f; for (vec2 v : bInsideA) {current += v.y - aMin.y;} if (current < best) {normal = vec2( 0.0f, -1.0f); for (int i = 0; i < 2; i++) {depths[i] = (bInsideA[i].y - aMin.y) * 0.5f; points[i] = bInsideA[i] + normal * depths[i]; best = current;}} // Bottom
                current = 0.0f; for (vec2 v : bInsideA) {current += aMax.x - v.x;} if (current < best) {normal = vec2( 1.0f,  0.0f); for (int i = 0; i < 2; i++) {depths[i] = (aMax.x - bInsideA[i].x) * 0.5f; points[i] = bInsideA[i] + normal * depths[i]; best = current;}} // Right
                current = 0.0f; for (vec2 v : bInsideA) {current += v.x - aMin.x;} if (current < best) {normal = vec2(-1.0f,  0.0f); for (int i = 0; i < 2; i++) {depths[i] = (bInsideA[i].x - aMin.x) * 0.5f; points[i] = bInsideA[i] + normal * depths[i]; best = current;}} // Left

                // Rotate back into global coordinates.
                rotate(normal, vec2(0.0f, 0.0f), aCos, aSin);
                rotate(points[0], aPos, aCos, aSin);
                rotate(points[1], aPos, aCos, aSin);
                if (flip) {normal = -normal;}

                result.push_back(CollisionManifold(normal, points[0], depths[0] * 0.5f));
                result.push_back(CollisionManifold(normal, points[1], depths[1] * 0.5f));
                return result;

            }

            return result;
        }

        std::vector<CollisionManifold> findCollisionFeaturesCircleAndBox(Circle* c, Box* b, bool flip) {

            std::vector<CollisionManifold> result;

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
            vec2 point;

            // Colliding with the top face of the box.
            if (cPos.x >= bMin.x && cPos.x <= bMax.x && cPos.y >= bMax.y && cPos.y < bMax.y + cRadius) {

                depth = (bMax.y - (cPos.y - cRadius)) * 0.5f;
                normal = vec2(0.0f, -1.0f);
                point = vec2(cPos.x, bMax.y - depth);
                if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(point, bPos, rCos, -rSin);}
                if (flip) {normal = -normal;}

                result.push_back(CollisionManifold(normal, point, depth));
                return result;
                
            }

            // Colliding with the bottom face of the box.
            if (cPos.x >= bMin.x && cPos.x <= bMax.x && cPos.y > bMin.y - cRadius && cPos.y <= bMin.y) {

                depth = ((cPos.y + cRadius) - bMin.y) * 0.5f;
                normal = vec2(0.0f, 1.0f);
                point = vec2(cPos.x, bMin.y + depth);
                if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(point, bPos, rCos, -rSin);}
                if (flip) {normal = -normal;}

                result.push_back(CollisionManifold(normal, point, depth));
                return result;
                
            }

            // Colliding with the right face of the box
            if (cPos.y >= bMin.y && cPos.y <= bMax.y && cPos.x >= bMax.x && cPos.x < bMax.x + cRadius) {

                depth = (bMax.x - (cPos.x - cRadius)) * 0.5f;
                normal = vec2(-1.0f, 0.0f);
                point = vec2(bMax.x - depth, cPos.y);
                if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(point, bPos, rCos, -rSin);}
                if (flip) {normal = -normal;}

                result.push_back(CollisionManifold(normal, point, depth));
                return result;
                
            }

            // Colliding with the left face of the box.
            if (cPos.y >= bMin.y && cPos.y <= bMax.y && cPos.x > bMin.x - cRadius && cPos.x <= bMin.x) {

                depth = ((cPos.x + cRadius) - bMin.x) * 0.5f;
                normal = vec2(1.0f, 0.0f);
                point = vec2(bMin.x + depth, cPos.y);
                if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(point, bPos, rCos, -rSin);}
                if (flip) {normal = -normal;}

                result.push_back(CollisionManifold(normal, point, depth));
                return result;
                
            }

            // Colliding with the top left corner of the box.
            if (glm::dot(vec2(cPos.x - bMin.x, cPos.y - bMax.y), vec2(cPos.x - bMin.x, cPos.y - bMax.y)) < cRadius * cRadius) {

                vec2 difference = vec2(bMin.x, bMax.y) - cPos;
                normal = glm::normalize(difference);
                vec2 depthVector = ((normal * cRadius) - difference) * 0.5f;            
                depth = glm::length(depthVector);
                point = vec2(bMin.x, bMax.y) + depthVector;
                if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(point, bPos, rCos, -rSin);}
                if (flip) {normal = -normal;}

                result.push_back(CollisionManifold(normal, point, depth));
                return result;
                
            }

            // Colliding with the top right corner of the box.
            if (glm::dot(vec2(cPos.x - bMax.x, cPos.y - bMax.y), vec2(cPos.x - bMax.x, cPos.y - bMax.y)) < cRadius * cRadius) {

                vec2 difference = vec2(bMax.x, bMax.y) - cPos;
                normal = glm::normalize(difference);
                vec2 depthVector = ((normal * cRadius) - difference) * 0.5f;            
                depth = glm::length(depthVector);
                point = vec2(bMax.x, bMax.y) + depthVector;
                if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(point, bPos, rCos, -rSin);}
                if (flip) {normal = -normal;}

                result.push_back(CollisionManifold(normal, point, depth));
                return result;
                
            }

            // Colliding with the bottom left corner of the box.
            if (glm::dot(vec2(cPos.x - bMin.x, cPos.y - bMin.y), vec2(cPos.x - bMin.x, cPos.y - bMin.y)) < cRadius * cRadius) {

                vec2 difference = vec2(bMin.x, bMin.y) - cPos;
                normal = glm::normalize(difference);
                vec2 depthVector = ((normal * cRadius) - difference) * 0.5f;            
                depth = glm::length(depthVector);
                point = vec2(bMin.x, bMin.y) + depthVector;
                if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(point, bPos, rCos, -rSin);}
                if (flip) {normal = -normal;}

                result.push_back(CollisionManifold(normal, point, depth));
                return result;
                
            }

            // Colliding with the bottom right corner of the box.
            if (glm::dot(vec2(cPos.x - bMax.x, cPos.y - bMin.y), vec2(cPos.x - bMax.x, cPos.y - bMin.y)) < cRadius * cRadius) {

                vec2 difference = vec2(bMax.x, bMin.y) - cPos;
                normal = glm::normalize(difference);
                vec2 depthVector = ((normal * cRadius) - difference) * 0.5f;            
                depth = glm::length(depthVector);
                point = vec2(bMax.x, bMin.y) + depthVector;
                if (bRot != 0.0f) {rotate(normal, vec2(0.0f, 0.0f), rCos, -rSin); rotate(point, bPos, rCos, -rSin);}
                if (flip) {normal = -normal;}

                result.push_back(CollisionManifold(normal, point, depth));
                return result;

            }
            
            return result;
        }

    }

    namespace Collision {
        
        std::vector<CollisionManifold> findCollisionFeatures(Collider* c1, Collider* c2) {

            if (dynamic_cast<Box*>(c1) != nullptr && dynamic_cast<Box*>(c2) != nullptr) {return findCollisionFeaturesBoxAndBox((Box*) c1, (Box*) c2);}
            if (dynamic_cast<Circle*>(c1) != nullptr && dynamic_cast<Circle*>(c2) != nullptr) {return findCollisionFeaturesCircleAndCircle((Circle*) c1, (Circle*) c2);}
            if (dynamic_cast<Circle*>(c1) != nullptr && dynamic_cast<Box*>(c2) != nullptr) {return findCollisionFeaturesCircleAndBox((Circle*) c1, (Box*) c2, false);}
            if (dynamic_cast<Box*>(c1) != nullptr && dynamic_cast<Circle*>(c2) != nullptr) {return findCollisionFeaturesCircleAndBox((Circle*) c2, (Box*) c1, true);}

            std::vector<CollisionManifold> empty;
            return empty;

        }

    }

}