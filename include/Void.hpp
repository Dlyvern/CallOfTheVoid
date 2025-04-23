#ifndef VOID_HPP
#define VOID_HPP

#include "Shader.hpp"
#include "Common.hpp"
#include "GameObject.hpp"
#include "SkinnedModel.hpp"

#include "Animator.hpp"
#include "Material.hpp"

class Void final : public GameObject
{
public:
    explicit Void(const std::string& name);

    void setModel(SkinnedModel* model);
    void update(float deltaTime) override;
    void rotate(bool rotateClockwise);
    void playAnimation();
    void setRigidBody(physx::PxRigidActor* rigidBody);
    physx::PxRigidActor* getRigidBody() const;

    void setPosition(const glm::vec3& position) override;
    void setScale(const glm::vec3& scale) override;

    Material getMaterial() const;

    SkinnedModel* getModel();

    void setMaterial(const Material& material);

    void calculateShadows(Shader& shader) override;

    Shader& getShader();

    ~Void() override;
private:
    bool m_rotate{false};

    glm::mat4 computeModelMatrix() const;

    Material m_material;
    common::Animation* m_animation{nullptr};
    physx::PxRigidActor* m_rigidBody{nullptr};

    Animator m_animator;

    SkinnedModel* m_model{nullptr};
    float m_rotationSpeed{50.0f};
    bool m_rotateClockwise{true};
};

#endif //VOID_HPP
