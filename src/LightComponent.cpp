#include "LightComponent.hpp"
#include "GameObject.hpp"

void LightComponent::update(float deltaTime)
{
    if (const auto owner = this->getOwner(); owner)
    {
        const auto position = owner->getPosition();
        m_light.position = position;
    }
}

lighting::Light* LightComponent::getLight()
{
    return &m_light;
}
