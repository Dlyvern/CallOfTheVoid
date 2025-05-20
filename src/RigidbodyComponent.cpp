#include "RigidbodyComponent.hpp"
#include "GameObject.hpp"

RigidbodyComponent::RigidbodyComponent() = default;

RigidbodyComponent::RigidbodyComponent(const std::shared_ptr<GameObject> &object)
{
    m_rigidActor = physics::PhysicsController::instance().addStaticActor(object);
    object->setPositionChangedCallback(std::bind(&RigidbodyComponent::onOwnerPositionChanged, this, std::placeholders::_1));
}

void RigidbodyComponent::update(float deltaTime)
{
    if (auto owner = this->getOwner())
    {
        const physx::PxTransform transform = m_rigidActor->getGlobalPose();
        owner->setPosition({transform.p.x, transform.p.y, transform.p.z});
    }
}

physx::PxRigidActor* RigidbodyComponent::getRigidActor() const
{
    return m_rigidActor;
}

void RigidbodyComponent::destroy()
{
    Component::destroy();

    if (m_rigidActor)
    {
        m_rigidActor->release();
        m_rigidActor = nullptr;
    }
}

void RigidbodyComponent::onOwnerPositionChanged(const glm::vec3& position)
{
    m_rigidActor->setGlobalPose({position.x, position.y, position.z});
}
