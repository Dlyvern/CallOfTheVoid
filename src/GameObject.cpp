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

const common::LayerMask& GameObject::getLayerMask() const
{
    return m_layerMask;
}

void GameObject::setRotation(float angle, const glm::vec3 &axis)
{
    m_rotation = axis;

    // if (axis == glm::vec3(1.0f, 0.0f, 0.0f))
    //     m_rotation.x += angle;
    // else if (axis == glm::vec3(0.0f, 1.0f, 0.0f))
    //     m_rotation.y += angle;
    // else if (axis == glm::vec3(0.0f, 0.0f, 1.0f))
    //     m_rotation.z += angle;
}

const std::string &GameObject::getName() const
{
    return m_name;
}

GameObject::~GameObject() = default;

glm::mat4 GameObject::getTransformMatrix() const
{
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_position);

    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
    glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
    glm::mat4 rotation = rotY * rotX * rotZ;

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale);

    return translation * rotation * scale;
}

void GameObject::setName(const std::string &name)
{
    m_name = name;
}

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
