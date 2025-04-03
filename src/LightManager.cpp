#include "LightManager.hpp"

LightManager & LightManager::instance()
{
    static LightManager instance;
    return instance;
}

void LightManager::addLight(const lighting::Light &light)
{
    m_lights.push_back(light);
}

void LightManager::sendLightsIntoShader(Shader &shader)
{
    auto light = m_lights.begin();
    shader.setVec3("light.position", light->position);
    shader.setVec3("light.color", light->color);
    shader.setFloat("light.strength", light->strength);
    shader.setFloat("light.radius", light->radius);

    // for (size_t i = 0; i < m_lights.size(); ++i)
    // {
    //     const lighting::Light& light = m_lights[i];
    //
    //     shader.setVec3("lights[" + std::to_string(i) + "].position", light.position);
    //     shader.setVec3("lights[" + std::to_string(i) + "].color", light.color);
    //     shader.setFloat("lights[" + std::to_string(i) + "].strength", light.strength);
    //     shader.setFloat("lights[" + std::to_string(i) + "].radius", light.radius);
    //
    //     if (light.type == lighting::LightType::SPOT)
    //     {
    //         shader.setVec3("lights[" + std::to_string(i) + "].direction", light.direction);
    //         shader.setFloat("lights[" + std::to_string(i) + "].cutoff", light.cutoff);
    //         shader.setFloat("lights[" + std::to_string(i) + "].outerCutoff", light.outerCutoff);
    //     }
    // }
}
