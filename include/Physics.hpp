#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <memory>

#include "PxConfig.h"
#include "PxPhysicsAPI.h"

#include "GameObject.hpp"

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
        physx::PxRigidStatic* addStaticActor(std::shared_ptr<GameObject> actor);

        void release();

        physx::PxControllerManager* getControllerManager();

        physx::PxMaterial* getDefaultMaterial();

        physx::PxScene* getScene();

        void createRagdoll(const std::vector<common::BoneInfo> &boneInfos, std::vector<common::RagdollBone>& ragdollBones)
        {
            for (int i = 0; i < boneInfos.size(); ++i) {
                physx::PxTransform pxTransform = physx::PxTransform(
                    physx::PxVec3(0, 5 + i * 0.5f, 0), physx::PxQuat(physx::PxIdentity));

                auto* body = m_physics->createRigidDynamic(pxTransform);
                auto* shape = m_physics->createShape(physx::PxCapsuleGeometry(0.1f, 0.3f), *m_physics->createMaterial(0.5f, 0.5f, 0.5f));
                body->attachShape(*shape);
                physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
                m_scene->addActor(*body);

                common::RagdollBone ragdollBone;

                ragdollBone.actor = body;
                ragdollBone.joint = nullptr;
                ragdollBone.bindTransform = glm::mat4(1.0f);
                ragdollBone.boneId = i;


                ragdollBones.push_back(ragdollBone);

                // Create joint with parent
                int parentIdx = boneInfos[i].parentId;

                if (parentIdx >= 0)
                {
                    physx::PxTransform parentTransform = body->getGlobalPose();
                    ragdollBones[i].joint =physx::PxD6JointCreate(*m_physics,
                        ragdollBones[parentIdx].actor, physx::PxTransform(physx::PxIdentity),
                        ragdollBones[i].actor, physx::PxTransform(physx::PxIdentity));
                }
            }
        }

        void updateBoneMatricesFromRagdoll(const std::vector<common::BoneInfo>& boneInfos, const std::vector<common::RagdollBone>& ragdollBones, std::vector<glm::mat4>& final)
        {
            for (int i = 0; i < boneInfos.size(); ++i)
                final.emplace_back(glm::mat4(1.0f));

            for (int i = 0; i < boneInfos.size(); ++i)
            {
                physx::PxTransform t = ragdollBones[i].actor->getGlobalPose();
                glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(t.p.x, t.p.y, t.p.z));
                final[i] = world * boneInfos[i].offsetMatrix;
            }
        }


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
