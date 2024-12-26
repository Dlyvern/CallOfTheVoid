#include "GameObject.hpp"

GameObject::GameObject(const std::string &name) : m_name(name)
{
    m_boundingBox.min = m_position - m_scale / 2.0f;
    m_boundingBox.max = m_position + m_scale / 2.0f;
}

void GameObject::setLayerMask(const common::LayerMask &layerMask)
{
    m_layerMask = layerMask;
}

const common::LayerMask &GameObject::getLayerMask() const
{
    return m_layerMask;
}

const std::string &GameObject::getName() const
{
    return m_name;
}

GameObject::~GameObject() = default;

void GameObject::setPosition(const glm::vec3 &position)
{
    m_position = position;
    m_boundingBox.min = m_position - m_scale / 2.0f;  
    m_boundingBox.max = m_position + m_scale / 2.0f;
}

void GameObject::setScale(const glm::vec3 &scale)
{
    m_scale = scale;
    m_boundingBox.min = m_position - m_scale / 2.0f;  
    m_boundingBox.max = m_position + m_scale / 2.0f;
}

glm::vec3 GameObject::getPosition() const
{
    return m_position;
}

glm::vec3 GameObject::getScale() const
{
    return m_scale;
}

const common::AABB &GameObject::getBoundingBox() const
{
    return m_boundingBox;
}
