#ifndef LIGHT_MANAGER_HPP
#define LIGHT_MANAGER_HPP

#include <vector>

#include "Light.hpp"
#include "Shader.hpp"

class LightManager
{
public:
    static LightManager& instance();

    void addLight(const lighting::Light& light);

    void sendLightsIntoShader(Shader& shader);

private:
    std::vector<lighting::Light> m_lights;

    LightManager() = default;
    LightManager(const LightManager&) = delete;
    LightManager& operator=(const LightManager&) = delete;
};

#endif //LIGHT_MANAGER_HPP
