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

void GameObject::setRotation(float angle, const glm::vec3 &axis)
{
    if (axis == glm::vec3(1.0f, 0.0f, 0.0f))
        m_rotation.x += angle;
    else if (axis == glm::vec3(0.0f, 1.0f, 0.0f))
        m_rotation.y += angle;
    else if (axis == glm::vec3(0.0f, 0.0f, 1.0f))
        m_rotation.z += angle;
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

glm::vec3 GameObject::getRotation() const
{
    return m_rotation;
}

const common::AABB &GameObject::getBoundingBox() const
{
    return m_boundingBox;
}
