#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <memory>

#include "PxConfig.h"
#include "PxPhysicsAPI.h"

#include "GameObject.hpp"
#include "Skeleton.hpp"

//TODO Make clear PxScene in different Scenes
namespace physics
{
    class PhysicsController
    {
    public:
        void init();
        static PhysicsController& instance();
        void simulate(float deltaTime);
        physx::PxRigidDynamic* addDynamicActor(std::shared_ptr<GameObject> actor);
        physx::PxRigidStatic* addStaticActor(std::shared_ptr<GameObject> actor, bool fixSize = false);

        void release();

        physx::PxControllerManager* getControllerManager();

        physx::PxMaterial* getDefaultMaterial();

        physx::PxScene* getScene();

        void createRagdoll(Skeleton& skeleton);
    private:
        physx::PxPhysics* m_physics{nullptr};
        physx::PxScene* m_scene{nullptr};
        physx::PxFoundation* m_foundation{nullptr};
        physx::PxDefaultAllocator m_defaultAllocator;
        physx::PxDefaultCpuDispatcher* m_cpuDispatcher{nullptr};
        physx::PxPvd* m_pvd{nullptr};
        physx::PxControllerManager* m_controllerManager{nullptr};

        physx::PxMaterial* m_defaultMaterial{nullptr};
    };
} //namespace physics

#endif //PHYSICS_HPP
