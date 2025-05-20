#include "GameObject.hpp"

GameObject::GameObject(const std::string &name) : m_name(name) {}

GameObject::~GameObject() = default;

void GameObject::setLayerMask(const common::LayerMask &layerMask)
{
    m_layerMask = layerMask;
}

void GameObject::setName(const std::string &name)
{
    m_name = name;
}

void GameObject::setRotation(const glm::vec3 &axis)
{
    m_rotation = axis;

    m_isTransformMatrixDirty = true;

    if (m_transformationChangedCallback)
        m_transformationChangedCallback(getTransformMatrix());
}

void GameObject::setPosition(const glm::vec3 &position)
{
    m_position = position;

    m_isTransformMatrixDirty = true;

    if (m_positionChangedCallback)
        m_positionChangedCallback(position);

    if (m_transformationChangedCallback)
        m_transformationChangedCallback(getTransformMatrix());
}

void GameObject::setScale(const glm::vec3 &scale)
{
    m_scale = scale;
    m_isTransformMatrixDirty = true;
    
    if (m_transformationChangedCallback)
        m_transformationChangedCallback(getTransformMatrix());
}

void GameObject::setPositionChangedCallback(const std::function<void(const glm::vec3 &)> &callback)
{
    m_positionChangedCallback = callback;
}

void GameObject::setTransformationChangedCallback(const std::function<void(const glm::mat4 &)> &callback)
{
    m_transformationChangedCallback = callback;
}

const common::LayerMask& GameObject::getLayerMask() const
{
    return m_layerMask;
}

const std::string &GameObject::getName() const
{
    return m_name;
}

glm::mat4 GameObject::getTransformMatrix()
{
    if (!m_isTransformMatrixDirty)
        return m_transformMatrix;

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_position);

    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
    glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
    glm::mat4 rotation = rotY * rotX * rotZ;

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale);

    m_transformMatrix = translation * rotation * scale;

    m_isTransformMatrixDirty = false;

    return m_transformMatrix;
}

void GameObject::setTransformMatrix(const glm::mat4 &transformMatrix)
{
    m_transformMatrix = transformMatrix;
    m_isTransformMatrixDirty = false;
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

void GameObject::destroy()
{
    for (auto& component : m_components)
        component.second->destroy();
}

void GameObject::update(float deltaTime)
{
    for (auto& [_, comp] : m_components)
        comp->update(deltaTime);
}