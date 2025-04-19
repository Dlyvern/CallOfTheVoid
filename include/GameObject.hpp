#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include "Common.hpp"
#include "Shader.hpp"

class GameObject
{
public:
    explicit GameObject(const std::string&name);

    virtual void setLayerMask(const common::LayerMask& layerMask);
    virtual void setPosition(const glm::vec3& position);
    virtual void setScale(const glm::vec3& scale);
    virtual void setRotation(float angle, const glm::vec3 &axis);

    [[nodiscard]] glm::vec3 getPosition() const;
    [[nodiscard]] glm::vec3 getScale() const;
    [[nodiscard]] glm::vec3 getRotation() const;
    [[nodiscard]] const common::AABB& getBoundingBox() const;
    [[nodiscard]] const common::LayerMask& getLayerMask() const;
    [[nodiscard]] const std::string& getName() const;

    virtual void update(float deltaTime) = 0;
    virtual void calculateShadows(Shader &shader) = 0;

    virtual ~GameObject();
private:
    common::LayerMask m_layerMask{common::LayerMask::DEFAULT};
    glm::vec3 m_position{glm::vec3(0.0f, 0.0f, 0.0f)};
    glm::vec3 m_scale{glm::vec3(1.0f, 1.0f, 1.0f)};
    glm::vec3 m_rotation{0.0f};
    common::AABB m_boundingBox;
    std::string m_name;
};

#endif //GAME_OBJECT_HPP