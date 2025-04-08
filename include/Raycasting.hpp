#ifndef RAYCASTING_HPP
#define RAYCASTING_HPP

#include <glm/vec3.hpp>
#include "Cube.hpp"
#include "Collision.hpp"

#include <Physics.hpp>

namespace physics
{
    namespace raycasting
    {
        struct RaycastingResult
        {
        public:
            glm::vec3 point;
            float distance;
            physx::PxRaycastBuffer hit;
            std::vector<std::shared_ptr<GameObject>> objects;
        };

        struct Ray
        {
        public:
            glm::vec3 origin{0.0f};
            glm::vec3 direction{0.0f, 0.0f, 1.0f};
            float maxDistance{0.1f};
            const std::vector<std::shared_ptr<GameObject>>* objects;
        };

        bool shoot(const Ray& ray, RaycastingResult& result);
    }; //namespace raycasting
}; //namespace physics

#endif //RAYCASTING_HPP