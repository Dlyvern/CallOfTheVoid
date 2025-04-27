#ifndef VOID_HPP
#define VOID_HPP

#include "GameObject.hpp"
#include "SkinnedModel.hpp"
#include "Animator.hpp"
#include "Physics.hpp"

class Void final : public GameObject
{
public:
    explicit Void(const std::string& name);

    void update(float deltaTime) override;
    void playAnimation();
    void calculateShadows(GLitch::Shader& shader) override;
    void destroy() override;

    void setPosition(const glm::vec3& position) override;
    void setScale(const glm::vec3& scale) override;
    void setModel(SkinnedModel* model);
    void setRigidBody(physx::PxRigidActor* rigidBody);

    SkinnedModel* getModel() const;
    physx::PxRigidActor* getRigidBody() const;

    ~Void() override;
private:
    glm::mat4 computeModelMatrix() const;

    common::Animation* m_animation{nullptr};
    physx::PxRigidActor* m_rigidBody{nullptr};
    Animator m_animator;
    SkinnedModel* m_model{nullptr};
};

#endif //VOID_HPP
