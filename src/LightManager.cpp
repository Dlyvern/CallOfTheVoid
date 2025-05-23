#include "LightManager.hpp"
#include <iostream>

LightManager& LightManager::instance()
{
    static LightManager instance;
    return instance;
}

void LightManager::addLight(lighting::Light* light)
{
    m_lights.push_back(light);
}

std::vector<lighting::Light*> LightManager::getLights()
{
    return m_lights;
}

lighting::Light* LightManager::getDirectionalLight() const
{
    for (const auto& light : m_lights)
        if (light->type == lighting::LightType::DIRECTIONAL)
            return light;

    return nullptr;
}

void LightManager::sendLightsIntoShader(GLitch::Shader &shader)
{
    for (size_t i = 0; i < m_lights.size(); ++i)
    {
        const lighting::Light* light = m_lights[i];
        shader.setInt("lights[" + std::to_string(i) + "].type", static_cast<int>(light->type));
        shader.setVec3("lights[" + std::to_string(i) + "].position", light->position);
        shader.setVec3("lights[" + std::to_string(i) + "].color", light->color);
        shader.setFloat("lights[" + std::to_string(i) + "].strength", light->strength);
        shader.setFloat("lights[" + std::to_string(i) + "].radius", light->radius);
        shader.setVec3("lights[" + std::to_string(i) + "].direction", light->direction);

        // if (light.type == lighting::LightType::SPOT)
        // {
        //     shader.setVec3("lights[" + std::to_string(i) + "].direction", light.direction);
        // }
    }
}

glm::mat4 LightManager::getLightSpaceMatrix() const
{
    return m_lightSpaceMatrix;
}

void LightManager::setLightSpaceMatrix(const glm::mat4 &matrix)
{
    m_lightSpaceMatrix = matrix;
}

void LightManager::bindGlobalLighting(GLitch::Shader &shader)
{
    shader.setMat4("lightSpaceMatrix", getLightSpaceMatrix());
    shader.setInt("shadowMap", 8);
    sendLightsIntoShader(shader);
}