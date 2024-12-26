#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <glm/vec3.hpp>
#include "Common.hpp"

namespace physics
{
    namespace collision
    {
        bool doAABBIntersectABBB(const common::AABB& firstAABB, const common::AABB& secondAABB);
        bool doRayIntersectAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& aabbMin, const glm::vec3& aabbMax, float& tNear, float& tFar);
        glm::vec3 calculatePenetrationDepth(const common::AABB& firstAABB, const common::AABB& secondAABB);
        float calculateIntersectionDepth(const common::AABB& firstAABB, const common::AABB& secondAABB);
    } //namespace collision
} //namespace physics

#endif //COLLISION_HPP