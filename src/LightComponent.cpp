#include "LightComponent.hpp"

void LightComponent::update(float deltaTime)
{

}

lighting::Light& LightComponent::getLight()
{
    return m_light;
}
