#ifndef LIGHT_COMPONENT_HPP
#define LIGHT_COMPONENT_HPP

#include "Component.hpp"
#include "Light.hpp"

class LightComponent final : public Component
{
public:
    LightComponent() = default;

    explicit LightComponent(const lighting::Light& light) : m_light(light) {}

    void update(float deltaTime) override;

    lighting::Light* getLight();
private:
    lighting::Light m_light{};
};

#endif //LIGHT_COMPONENT_HPP
