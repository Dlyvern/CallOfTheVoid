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
            std::vector<std::shared_ptr<GameObject>> objects;
        };

        struct Ray
        {
        public:
            glm::vec3 origin;
            glm::vec3 direction;
            const std::vector<std::shared_ptr<GameObject>>* objects;
        };

        inline bool shoot(const Ray& ray, RaycastingResult& result)
        {
            float closestT = std::numeric_limits<float>::max();

            for (const auto& gameObject : *ray.objects)
            {
                //TODO FIX THIS LATER
                if (gameObject->getName() == "player")
                    continue;

                float tNear, tFar;

                if (physics::collision::doRayIntersectAABB(ray.origin, ray.direction, gameObject->getBoundingBox().min, gameObject->getBoundingBox().max, tNear, tFar))
                {
                    if (tNear < closestT)
                    {
                        //Some weird shit
                        // closestT = tNear;

                        result.objects.push_back(gameObject);
                        std::cout << "Hit game object with " << gameObject->getName() << " name" << std::endl;
                    }
                }
            }

            return !result.objects.empty();
        }
    }; //namespace raycasting
}; //namespace physics

#endif //RAYCASTING_HPP