#include "Void.hpp"

#include <glad.h>

#include "Filesystem.hpp"
#include "CameraManager.hpp"
#include "LightManager.hpp"
#include "Physics.hpp"
#include "WindowsManager.hpp"

std::vector<common::RagdollBone> RagdollBones;
// unsigned int depthMapFBO;
// unsigned int depthMap;
//
// const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

Void::Void(const std::string &name) : GameObject(name)
{
}

void Void::create()
{
    const std::string& shadersPath = filesystem::getShadersFolderPath().string();

    m_shader.load(shadersPath + "/skeleton.vert", shadersPath + "/skeleton.frag");

    m_shadowShader.load(shadersPath + "/shadow.vert", shadersPath + "/shadow.frag");

    // glGenFramebuffers(1, &depthMapFBO);
    // glGenTextures(1, &depthMap);
    // glBindTexture(GL_TEXTURE_2D, depthMap);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
    //              GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    //
    // float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    //
    // glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Void::update(float deltaTime)
{
     m_animator.updateAnimation(deltaTime);

     m_shader.bind();

     if(m_rotate)
     {
         if (m_rotateClockwise)
             m_rotation += m_rotationSpeed * deltaTime;
         else
             m_rotation -= m_rotationSpeed * deltaTime;

         // m_rotation = fmod(m_rotation, 360.0f);
         m_rotate = false;
     }

     glm::mat4 model(1.0f);

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
     model = glm::translate(model, getPosition());
     model = glm::scale(model, getScale());
     const glm::mat4 rotationMatrix =
         glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
         glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
         glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

     model *= rotationMatrix;
     const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

     m_shader.setMat4("model", model);
     m_shader.setMat4("view", CameraManager::getInstance().getActiveCamera()->getViewMatrix());
     m_shader.setMat4("projection", projection);

     LightManager::instance().sendLightsIntoShader(m_shader);

     m_material.bind(m_shader);

     m_shader.setInt("texture_diffuse1", 0);

     m_model->draw();


    //
    //
    // const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    //
    // // Light matrices
    // glm::vec3 lightPos = LightManager::instance().getLights()[0].position;
    // glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 100.0f);
    // glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    // glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    //
    // // Update animator
    // m_animator.updateAnimation(deltaTime);
    //
    // // Get bone matrices
    // std::vector<glm::mat4> boneMatrices;
    // if (m_animator.isAnimationPlaying())
    //     boneMatrices = m_animator.getFinalBoneMatrices();
    // else
    //     m_model->getSkeleton().calculateBindPoseTransforms(boneMatrices);
    //
    // // Set model matrix
    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, getPosition());
    // const glm::mat4 rotationMatrix =
    //     glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
    //     glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
    //     glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    //
    // model *= rotationMatrix;
    // model = glm::scale(model, getScale());
    //
    // // --- Shadow pass ---
    // glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    // glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    // glClear(GL_DEPTH_BUFFER_BIT);
    //
    // m_shadowShader.bind();
    // m_shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    // m_shadowShader.setMat4("model", model);
    //
    // for (unsigned int i = 0; i < boneMatrices.size(); ++i)
    // {
    //     std::string name = "finalBonesMatrices[" + std::to_string(i) + "]";
    //     m_shadowShader.setMat4(name, boneMatrices[i]);
    // }
    //
    // m_model->draw();
    //
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //
    // auto* win = window::WindowsManager::instance().getCurrentWindow();
    // float aspect = static_cast<float>(win->getWidth()) / win->getHeight();
    // glViewport(0, 0, win->getWidth(), win->getHeight());
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    // glm::mat4 view = CameraManager::getInstance().getActiveCamera()->getViewMatrix();
    //
    // m_shader.bind();
    // m_shader.setMat4("projection", projection);
    // m_shader.setMat4("view", view);
    // m_shader.setMat4("model", model);
    // m_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    //
    // // Bone matrices again
    // for (unsigned int i = 0; i < boneMatrices.size(); ++i)
    // {
    //     std::string name = "finalBonesMatrices[" + std::to_string(i) + "]";
    //     m_shader.setMat4(name, boneMatrices[i]);
    // }
    //
    // // Material textures
    // m_material.bind(m_shader); // Should bind Diffuse, Normal, Specular, AO to 0-3
    //
    // // Bind depth map to texture unit 4
    // glActiveTexture(GL_TEXTURE4);
    // glBindTexture(GL_TEXTURE_2D, depthMap);
    // m_shader.setUniform1i("shadowMap", 4);
    //
    // // Lights and uniforms
    // LightManager::instance().sendLightsIntoShader(m_shader);
    //
    // // Draw!
    // m_model->draw();

}

void Void::setRotation(float angle, const glm::vec3 &axis)
{
    if (axis == glm::vec3(1.0f, 0.0f, 0.0f))
        m_rotation.x += angle;
    else if (axis == glm::vec3(0.0f, 1.0f, 0.0f))
        m_rotation.y += angle;
    else if (axis == glm::vec3(0.0f, 0.0f, 1.0f))
        m_rotation.z += angle;
}

void Void::rotate(bool rotateClockwise)
{
    m_rotate = true;
    m_rotateClockwise = rotateClockwise;
}


void Void::setModel(SkinnedModel* model)
{
    m_model = model;
    // physics::PhysicsController::instance().createRagdoll(m_model->getSkeleton().getBones(), RagdollBones);
}

void Void::playAnimation()
{
    if (m_model->getAnimations().empty())
        return;

    m_animation = &m_model->getAnimations()[0];
    m_animation->modelBones = &m_model->getSkeleton().getBones();

    m_animator.playAnimation(m_animation, true);
}

void Void::setMaterial(const Material &material)
{
    m_material = material;
}

Void::~Void() = default;










//
// void Void::update(float deltaTime)
// {
//     m_animator.updateAnimation(deltaTime);
//
//     m_shader.bind();
//
//     if(m_rotate)
//     {
//         if (m_rotateClockwise)
//             m_rotation += m_rotationSpeed * deltaTime;
//         else
//             m_rotation -= m_rotationSpeed * deltaTime;
//
//         // m_rotation = fmod(m_rotation, 360.0f);
//         m_rotate = false;
//     }
//
//     glm::mat4 model(1.0f);
//
//     std::vector<glm::mat4> boneMatrices;
//
//     if (m_animator.isAnimationPlaying())
//         boneMatrices = m_animator.getFinalBoneMatrices();
//
//     else
//     {
//         // physics::PhysicsController::instance().updateBoneMatricesFromRagdoll(m_model->getSkeleton().getBones(), RagdollBones, boneMatrices);
//         m_model->getSkeleton().calculateBindPoseTransforms(boneMatrices);
//     }
//
//     for (unsigned int i = 0; i < boneMatrices.size(); ++i)
//     {
//         std::string uniformName = "finalBonesMatrices[" + std::to_string(i) + "]";
//         m_shader.setMat4(uniformName, boneMatrices[i]);
//     }
//
//     const auto* currentWindow = window::WindowsManager::instance().getCurrentWindow();
//     const float aspect = static_cast<float>(currentWindow->getWidth()) / currentWindow->getHeight();
//     model = glm::translate(model, getPosition());
//     model = glm::scale(model, getScale());
//     const glm::mat4 rotationMatrix =
//         glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
//         glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
//         glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
//
//     model *= rotationMatrix;
//     const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
//
//     m_shader.setMat4("model", model);
//     m_shader.setMat4("view", CameraManager::getInstance().getActiveCamera()->getViewMatrix());
//     m_shader.setMat4("projection", projection);
//
//     LightManager::instance().sendLightsIntoShader(m_shader);
//
//     m_material.bind(m_shader);
//
//     m_shader.setUniform1i("texture_diffuse1", 0);
//
//     m_model->draw();
// }
