#include "Void.hpp"

#include <iostream>

#include "LightManager.hpp"
#include "Renderer.hpp"
#include "ShaderManager.hpp"

Void::Void(const std::string &name) : GameObject(name) {}

glm::mat4 Void::computeModelMatrix() const
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, getPosition());
    model = glm::scale(model, getScale());
    const glm::mat4 rotationMatrix =
        glm::rotate(glm::mat4(1.0f), glm::radians(getRotation().x), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(getRotation().y), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(getRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));

    model *= rotationMatrix;

    return model;
}

void Void::calculateShadows(GLitch::Shader &shader)
{
    shader.setMat4("model", computeModelMatrix());

    std::vector<glm::mat4> boneMatrices;

    if (m_animator.isAnimationPlaying())
        boneMatrices = m_animator.getFinalBoneMatrices();

    else
        m_model->getSkeleton().calculateBindPoseTransforms(boneMatrices);

    for (int i = 0; i < boneMatrices.size(); ++i)
        shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", boneMatrices[i]);

    m_model->draw();
}

void Void::destroy()
{
    GameObject::destroy();

    if (m_rigidBody)
    {
        m_rigidBody->release();
        m_rigidBody = nullptr;
    }
}

void Void::update(float deltaTime)
{
    m_animator.updateAnimation(deltaTime);

    const auto shader = ShaderManager::instance().getShader(ShaderManager::ShaderType::SKELETON);
    shader->bind();

    std::vector<glm::mat4> boneMatrices;

    if (m_animator.isAnimationPlaying())
        boneMatrices = m_animator.getFinalBoneMatrices();
    else
        m_model->getSkeleton().calculateBindPoseTransforms(boneMatrices);

    for (unsigned int i = 0; i < boneMatrices.size(); ++i)
        shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", boneMatrices[i]);

    const auto& frameData = Renderer::instance().getFrameData();
    shader->setMat4("model", computeModelMatrix());
    shader->setMat4("view", frameData.viewMatrix);
    shader->setMat4("projection", frameData.projectionMatrix);

    LightManager::instance().sendLightsIntoShader(*shader);

    // m_model->draw();

    for (int index = 0; index < m_model->getMeshesSize(); index++)
    {
        auto& mesh = m_model->getMeshes()[index];

        Material* material;

        if (overrideMaterials.contains(index))
            material = overrideMaterials[index];
        else
            material = mesh.getMaterial();

        material->bind(*shader);

        mesh.draw();
    }
}

void Void::setModel(SkinnedModel* model)
{
    m_model = model;
    physics::PhysicsController::instance().createRagdoll(m_model->getSkeleton());
}

void Void::playAnimation(common::Animation* animation)
{
    if (animation)
    {
        m_animator.stopAnimation();
        m_animation = animation;
        m_animation->modelBones = &m_model->getSkeleton().getBones();
        m_animator.playAnimation(m_animation, true);
        return;
    }

    if (m_model->getAnimations().empty())
        return;

    m_animation = &m_model->getAnimations()[0];
    m_animation->modelBones = &m_model->getSkeleton().getBones();

    m_animator.playAnimation(m_animation, true);
}

void Void::setRigidBody(physx::PxRigidActor *rigidBody)
{
    m_rigidBody = rigidBody;
}

physx::PxRigidActor* Void::getRigidBody() const
{
    return m_rigidBody;
}

void Void::setPosition(const glm::vec3 &position)
{
    GameObject::setPosition(position);

    if (m_rigidBody)
        m_rigidBody->setGlobalPose({position.x, position.y, position.z});
}

void Void::setScale(const glm::vec3 &scale)
{
    GameObject::setScale(scale);
}

SkinnedModel* Void::getModel() const
{
    return m_model;
}

Void::~Void() = default;
