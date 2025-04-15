#include "Raycasting.hpp"

bool physics::raycasting::shoot(const Ray &ray, RaycastingResult &result)
{
    glm::vec3 direction = glm::normalize(ray.direction);
    const glm::vec3& origin = ray.origin;

    physx::PxQueryFilterData filterData;
    filterData.flags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC;
    filterData.data.word0 = common::LayerMask::DEFAULT | common::LayerMask::INTERACTABLE | common::LayerMask::GROUND;

    physx::PxRaycastBuffer hit;

    const auto scene = physics::PhysicsController::instance().getScene();

    bool status = scene->raycast({origin.x, origin.y, origin.z}, {direction.x, direction.y, direction.z}, ray.maxDistance, hit, physx::PxHitFlag::eDEFAULT, filterData);

    result.hit = hit;

    return status && hit.hasBlock;
}
