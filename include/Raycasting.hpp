#ifndef RAYCASTING_HPP
#define RAYCASTING_HPP

#include <glm/vec3.hpp>
#include "Cube.hpp"
#include "Collision.hpp"

namespace physics
{
    namespace raycasting
    {
        struct RaycastingResult
        {
        public:
            glm::vec3 point;
            float distance;
        };

        struct Ray
        {
        public:
            glm::vec3 origin;
            glm::vec3 direction;
        };

        inline bool shoot(const Ray& ray, RaycastingResult& result, const geometries::Cube &cube)
        {
            float closestT = std::numeric_limits<float>::max();

            float tNear, tFar;

            if (physics::collision::doRayIntersectAABB(ray.origin, ray.direction, cube.getBoundingBox().min, cube.getBoundingBox().max, tNear, tFar)) 
            {
                if (tNear < closestT) 
                {
                    closestT = tNear;

                    return true;
                }
            }

            return false;
        }
    }; //namespace raycasting
}; //namespace physics

#endif //RAYCASTING_HPP