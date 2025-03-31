#include "Void.hpp"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "Filesystem.hpp"

#include "CameraManager.hpp"


Void::Void(const std::string &name) : GameObject(name)
{
}

void Void::create()
{
    auto shadersPath = filesystem::getShadersFolderPath().string();

    m_shader.load(shadersPath + "/skelet.vert", shadersPath + "/skelet.frag");
}

void Void::setTexture(textures::Texture *texture)
{
    m_texture = texture;
}

void Void::setModel(SkinnedModel *model)
{
    m_model = model;

    m_animation = &model->getAnimations()[0];
    m_animation->modelBones = &model->getSkeleton().getBones();

    m_animator.playAnimation(m_animation, true);
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
    glm::mat4 projection(1.0f);

    for (unsigned int i = 0; i < m_animator.getFinalBoneMatrices().size(); ++i)
    {
        std::string uniformName = "finalBonesMatrices[" + std::to_string(i) + "]";
        m_shader.setMat4(uniformName, m_animator.getFinalBoneMatrices()[i]);
    }

    model = glm::translate(model, getPosition());
    model = glm::scale(model, getScale());
    glm::mat4 rotationMatrix =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    model *= rotationMatrix;
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(1920) / 1080, 0.1f, 100.0f);

    m_shader.setMat4("model", model);
    m_shader.setMat4("view", CameraManager::getInstance().getActiveCamera()->getViewMatrix());
    m_shader.setMat4("projection", projection);

    m_texture->bind(0);

    m_shader.setUniform1i("texture_diffuse1", 0);

    m_model->draw();
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

Void::~Void() = default;
