#ifndef LIGHT_MANAGER_HPP
#define LIGHT_MANAGER_HPP

#include "Light.hpp"
#include "Shader.hpp"
#include <vector>

class LightManager
{
public:
    static LightManager& instance();

    void addLight(lighting::Light* light);

    void sendLightsIntoShader(GLitch::Shader& shader);

    [[nodiscard]] glm::mat4 getLightSpaceMatrix() const;

    void setLightSpaceMatrix(const glm::mat4& matrix);

    void bindGlobalLighting(GLitch::Shader& shader);

    std::vector<lighting::Light*> getLights();

    [[nodiscard]] lighting::Light* getDirectionalLight() const;
private:
    std::vector<lighting::Light*> m_lights;

    glm::mat4 m_lightSpaceMatrix;

    LightManager() = default;
    LightManager(const LightManager&) = delete;
    LightManager& operator=(const LightManager&) = delete;
};

#endif //LIGHT_MANAGER_HPP
