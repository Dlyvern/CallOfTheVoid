#include "Collision.hpp"

bool physics::collision::doAABBIntersectABBB(const common::AABB& firstAABB, const common::AABB& secondAABB)
{
    return (firstAABB.min.x <= secondAABB.max.x && secondAABB.min.x <= firstAABB.max.x) &&
        (firstAABB.min.y <= secondAABB.max.y && secondAABB.min.y <= firstAABB.max.y) &&
        (firstAABB.min.z <= secondAABB.max.z && secondAABB.min.z <= firstAABB.max.z);
}

bool physics::collision::doRayIntersectAABB(const glm::vec3 &rayOrigin, const glm::vec3 &rayDir, const glm::vec3 &aabbMin, const glm::vec3 &aabbMax, float &tNear, float &tFar)
{
    tNear = -std::numeric_limits<float>::max();
    tFar = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; i++)
    {
        if (rayDir[i] != 0.0f)
        {
            float t1 = (aabbMin[i] - rayOrigin[i]) / rayDir[i];
            float t2 = (aabbMax[i] - rayOrigin[i]) / rayDir[i];

            // Ensure t1 is the smaller value
            if (t1 > t2)
            std::swap(t1, t2);

            tNear = std::max(tNear, t1);
            tFar = std::min(tFar, t2);

            if (tNear > tFar)
                return false;
        }
        else if (rayOrigin[i] < aabbMin[i] || rayOrigin[i] > aabbMax[i])
            return false; // Ray is parallel and outside the box
    }

    return true;
}

glm::vec3 physics::collision::calculatePenetrationDepth(const common::AABB& firstAABB, const common::AABB& secondAABB)
{
    // return displacement;
    float depthX = std::min(firstAABB.max.x - secondAABB.min.x, secondAABB.max.x - firstAABB.min.x);
    float depthY = std::min(firstAABB.max.y - secondAABB.min.y, secondAABB.max.y - firstAABB.min.y);
    float depthZ = std::min(firstAABB.max.z - secondAABB.min.z, secondAABB.max.z - firstAABB.min.z);

    return glm::vec3(depthX, depthY, depthZ);


    // glm::vec3 displacement = glm::vec3(0);

    // if (firstAABB.min.x < secondAABB.max.x && firstAABB.max.x > secondAABB.min.x) {
    //     displacement.x = std::max(secondAABB.min.x - firstAABB.max.x, firstAABB.min.x - secondAABB.max.x);
    // }
    // if (firstAABB.min.y < secondAABB.max.y && firstAABB.max.y > secondAABB.min.y) {
    //     displacement.y = std::max(secondAABB.min.y - firstAABB.max.y, firstAABB.min.y - secondAABB.max.y);
    // }
    // if (firstAABB.min.z < secondAABB.max.z && firstAABB.max.z > secondAABB.min.z) {
    //     displacement.z = std::max(secondAABB.min.z - firstAABB.max.z, firstAABB.min.z - secondAABB.max.z);
    // }
}

float physics::collision::calculateIntersectionDepth(const common::AABB &firstAABB, const common::AABB &secondAABB)
{
    // Calculate intersection depth along each axis
    float depthX = std::min(firstAABB.max.x - secondAABB.min.x, secondAABB.max.x - firstAABB.min.x);
    float depthY = std::min(firstAABB.max.y - secondAABB.min.y, secondAABB.max.y - firstAABB.min.y);
    float depthZ = std::min(firstAABB.max.z - secondAABB.min.z, secondAABB.max.z - firstAABB.min.z);

    // Return the minimum depth to detect how deeply they intersect
    return std::min(depthX, std::min(depthY, depthZ));
}
