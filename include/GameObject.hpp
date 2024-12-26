#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#pragma once

#include <glm/vec3.hpp>
#include "Common.hpp"
#include <string>

class GameObject
{
public:
    explicit GameObject(const std::string&name);

    void setLayerMask(const common::LayerMask& layerMask);
    virtual void setPosition(const glm::vec3& position);
    void setScale(const glm::vec3& scale);

    glm::vec3 getPosition() const;
    glm::vec3 getScale() const;
    const common::AABB& getBoundingBox() const;
    const common::LayerMask& getLayerMask() const;
    const std::string& getName() const;

    virtual void update(const glm::mat4& viewMatrix, const glm::vec3& cameraPosition, float deltaTime) = 0;

    virtual ~GameObject();
private:
    common::LayerMask m_layerMask{common::LayerMask::DEFAULT};
    glm::vec3 m_position{glm::vec3(0.0f, 0.0f, 0.0f)};
    glm::vec3 m_scale{glm::vec3(1.0f, 1.0f, 1.0f)};
    common::AABB m_boundingBox;
    std::string m_name;
};

#endif //GAME_OBJECT_HPP