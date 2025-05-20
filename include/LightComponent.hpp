#ifndef LIGHT_COMPONENT_HPP
#define LIGHT_COMPONENT_HPP

#include <glm/mat4x4.hpp>

#include "Component.hpp"
#include "Light.hpp"

class LightComponent final : public Component
{
public:
    explicit LightComponent(const lighting::Light& light);

    void update(float deltaTime) override;

    lighting::Light* getLight();

    void setOwner(GameObject* owner) override;
private:
    lighting::Light m_light{};

    void onTransformationOwnerChanged(const glm::mat4& transformation);
};

#endif //LIGHT_COMPONENT_HPP