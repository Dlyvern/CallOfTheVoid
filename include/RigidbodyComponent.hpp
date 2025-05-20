#ifndef RIGID_BODY_COMPONENT_HPP
#define RIGID_BODY_COMPONENT_HPP

#include <memory>

#include "Component.hpp"
#include "Physics.hpp"

class RigidbodyComponent final : public Component
{
public:
    RigidbodyComponent();

    explicit RigidbodyComponent(const std::shared_ptr<GameObject>& object);

    void update(float deltaTime) override;

    physx::PxRigidActor* getRigidActor() const;

    void destroy() override;
private:
    physx::PxRigidActor* m_rigidActor{nullptr};

    void onOwnerPositionChanged(const glm::vec3& postion);
};

#endif //RIGID_BODY_COMPONENT_HPP
