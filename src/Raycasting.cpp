#include "Raycasting.hpp"

bool physics::raycasting::shoot(const Ray &ray, RaycastingResult &result)
{
    glm::vec3 direction = glm::normalize(ray.direction);
    const glm::vec3& origin = ray.origin;

    physx::PxRaycastBuffer hit;

    auto scene = physics::PhysicsController::instance().getScene();

    bool status = scene->raycast({origin.x, origin.y, origin.z}, {direction.x, direction.y, direction.z}, ray.maxDistance, hit);

    result.hit = hit;

    return status && hit.hasBlock;
}
