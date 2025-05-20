#include "LightComponent.hpp"
#include "GameObject.hpp"

LightComponent::LightComponent(const lighting::Light &light) : m_light(light)
{

}

void LightComponent::update(float deltaTime) {}

lighting::Light* LightComponent::getLight()
{
    return &m_light;
}

void LightComponent::setOwner(GameObject *owner)
{
    Component::setOwner(owner);

    owner->setTransformationChangedCallback(std::bind(&LightComponent::onTransformationOwnerChanged, this, std::placeholders::_1));
}

void LightComponent::onTransformationOwnerChanged(const glm::mat4 &transformation)
{
    glm::vec3 forward = glm::normalize(glm::vec3(transformation[2]));
    m_light.direction = -forward;

    if (const auto owner = this->getOwner(); owner)
    {
        const auto position = owner->getPosition();
        m_light.position = position;
    }
}
