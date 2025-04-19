#include "Physics.hpp"
#include <iostream>

struct UserErrorCallback final : physx::PxErrorCallback
{
     void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, const int line) override
     {
         std::cout << file << " line " << line << ": " << message << "\n" << "\n";
     }
} gErrorCallback;

void physics::PhysicsController::init()
{
    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocator, gErrorCallback);
    m_pvd = PxCreatePvd(*m_foundation);

    physx::PxTolerancesScale scale;
    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, scale, true, m_pvd);

    physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    m_scene = m_physics->createScene(sceneDesc);

    m_defaultMaterial = m_physics->createMaterial(0.5f, 0.5f, 0.6f);

    m_controllerManager = PxCreateControllerManager(*m_scene);
}

physics::PhysicsController& physics::PhysicsController::instance()
{
    static physics::PhysicsController instance;
    return instance;
}

void physics::PhysicsController::simulate(float deltaTime)
{
    m_scene->simulate(deltaTime);
    m_scene->fetchResults(true);
}

void physics::PhysicsController::release()
{
    m_scene->release();
    m_physics->release();
    m_foundation->release();
}

physx::PxControllerManager* physics::PhysicsController::getControllerManager()
{
    return m_controllerManager;
}

physx::PxMaterial* physics::PhysicsController::getDefaultMaterial()
{
    return m_defaultMaterial;
}

physx::PxScene* physics::PhysicsController::getScene()
{
    return m_scene;
}

void physics::PhysicsController::createRagdoll(Skeleton &skeleton)
{
    auto &bones = skeleton.getBones();

    for (auto& bone : bones)
    {
        auto pos = glm::vec3(bone.globalBindTransform[3]);

        physx::PxTransform transform(physx::PxVec3(pos.x, pos.y, pos.z));

        physx::PxRigidDynamic* body = m_physics->createRigidDynamic(transform);
        physx::PxCapsuleGeometry capsule(0.05f, 0.15f);
        physx::PxShape* shape = m_physics->createShape(capsule, *m_defaultMaterial);
        body->attachShape(*shape);
        m_scene->addActor(*body);

        bone.rigidBody = body;
    }

    for (auto& child : bones)
    {
        if (child.parentId == -1)
            continue;

        auto& parent = bones[child.parentId];

        glm::vec3 p0 = glm::vec3(parent.globalBindTransform[3]);
        glm::vec3 p1 = glm::vec3(child.globalBindTransform[3]);
        glm::vec3 center = (p0 + p1) * 0.5f;

        glm::vec3 localA = p0 - center;
        glm::vec3 localB = p1 - center;

        physx::PxTransform frameA(physx::PxVec3(localA.x, localA.y, localA.z));
        physx::PxTransform frameB(physx::PxVec3(localB.x, localB.y, localB.z));

        auto joint = PxD6JointCreate(*m_physics,
            parent.rigidBody, frameA,
            child.rigidBody, frameB);

        joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLIMITED);
        joint->setSwingLimit(physx::PxJointLimitCone(physx::PxPi / 6, physx::PxPi / 6));
    }
}

physx::PxRigidDynamic* physics::PhysicsController::addDynamicActor(std::shared_ptr<GameObject> actor)
{
   physx:: PxMaterial* material = getDefaultMaterial();

    physx::PxTransform transform(physx::PxVec3(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z));
    physx::PxRigidDynamic* box = m_physics->createRigidDynamic(transform);

    physx::PxShape* shape = m_physics->createShape(physx::PxBoxGeometry(actor->getScale().x * 0.5f, actor->getScale().y * 0.5f, actor->getScale().z * 0.5f), *material);
    shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
    shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
    physx::PxFilterData filterData;
    filterData.word0 = actor->getLayerMask();
    shape->setSimulationFilterData(filterData);
    shape->setQueryFilterData(filterData);

    box->attachShape(*shape);
    box->userData = actor.get();

    physx::PxRigidBodyExt::updateMassAndInertia(*box, 10.0f);

    m_scene->addActor(*box);

    return box;
}

physx::PxRigidStatic * physics::PhysicsController::addStaticActor(std::shared_ptr<GameObject> actor, bool fixSize)
{
    physx::PxMaterial* material = getDefaultMaterial();
    physx::PxTransform transform(physx::PxVec3(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z));
    physx::PxRigidStatic* staticBody = m_physics->createRigidStatic(transform);

    physx::PxShape* shape;
    if (!fixSize)
        shape = m_physics->createShape(physx::PxBoxGeometry(actor->getScale().x * 0.5f, actor->getScale().y * 0.5f, actor->getScale().z * 0.5f),  *material);
    else
        shape = m_physics->createShape(physx::PxBoxGeometry(1.0f, 2.0f, 1.0f),  *material);


    shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true); // Enable raycasting
    shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);   // Optional if you want physics
    physx::PxFilterData filterData;
    filterData.word0 = actor->getLayerMask();
    shape->setSimulationFilterData(filterData);
    shape->setQueryFilterData(filterData);

    staticBody->attachShape(*shape);
    staticBody->userData = actor.get();

    m_scene->addActor(*staticBody);

    return staticBody;
}

// physx::PxFilterData filterData;
// filterData.word0 = actor->getLayerMask();     // The layer this object belongs to
// filterData.word1 = common::LayerMask::DEFAULT | common::LayerMask::GROUND;  // What this object can collide/interact with
//
// shape->setSimulationFilterData(filterData);
// shape->setQueryFilterData(filterData);

