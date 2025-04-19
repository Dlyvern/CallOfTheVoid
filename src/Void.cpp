#include "Void.hpp"

#include "Filesystem.hpp"
#include "CameraManager.hpp"
#include "LightManager.hpp"
#include "Physics.hpp"
#include "WindowsManager.hpp"

Void::Void(const std::string &name) : GameObject(name)
{
}

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

void Void::calculateShadows(Shader &shader)
{
    shader.setMat4("model", computeModelMatrix());

    std::vector<glm::mat4> boneMatrices;

    if (m_animator.isAnimationPlaying())
        boneMatrices = m_animator.getFinalBoneMatrices();

    else
        // m_model->getSkeleton().calculateRagdollBoneMatrices(boneMatrices);
        m_model->getSkeleton().calculateBindPoseTransforms(boneMatrices);

    for (int i = 0; i < boneMatrices.size(); ++i)
        shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", boneMatrices[i]);

    m_model->draw();
}

void Void::create()
{
    const std::string& shadersPath = filesystem::getShadersFolderPath().string();

    m_shader.load(shadersPath + "/skeleton.vert", shadersPath + "/skeleton.frag");
}

void Void::update(float deltaTime)
{
     m_animator.updateAnimation(deltaTime);

     m_shader.bind();

     std::vector<glm::mat4> boneMatrices;

     if (m_animator.isAnimationPlaying())
         boneMatrices = m_animator.getFinalBoneMatrices();

     else
     {
         // physics::PhysicsController::instance().updateBoneMatricesFromRagdoll(m_model->getSkeleton().getBones(), RagdollBones, boneMatrices);
         m_model->getSkeleton().calculateBindPoseTransforms(boneMatrices);
     }

     for (unsigned int i = 0; i < boneMatrices.size(); ++i)
     {
         std::string uniformName = "finalBonesMatrices[" + std::to_string(i) + "]";
         m_shader.setMat4(uniformName, boneMatrices[i]);
     }

     const auto* currentWindow = window::WindowsManager::instance().getCurrentWindow();
     const float aspect = static_cast<float>(currentWindow->getWidth()) / currentWindow->getHeight();
     const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

     m_shader.setMat4("model", computeModelMatrix());
     m_shader.setMat4("view", CameraManager::getInstance().getActiveCamera()->getViewMatrix());
     m_shader.setMat4("projection", projection);

     LightManager::instance().sendLightsIntoShader(m_shader);

     m_material.bind(m_shader);

     m_shader.setInt("shadowMap", 1);

     m_model->draw();
}

void Void::rotate(bool rotateClockwise)
{
    m_rotate = true;
    m_rotateClockwise = rotateClockwise;
}


void Void::setModel(SkinnedModel* model)
{
    m_model = model;
    physics::PhysicsController::instance().createRagdoll(m_model->getSkeleton());
}

void Void::playAnimation()
{
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

physx::PxRigidActor * Void::getRigidBody() const
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

SkinnedModel* Void::getModel()
{
    return m_model;
}

void Void::setMaterial(const Material &material)
{
    m_material = material;
}

Void::~Void() = default;
