#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "Common.hpp"
#include "Component.hpp"
#include "Material.hpp"
#include "ScriptsRegister.hpp"

class GameObject
{
public:
    explicit GameObject(const std::string&name);

    virtual void setLayerMask(const common::LayerMask& layerMask);
    virtual void setPosition(const glm::vec3& position);
    virtual void setScale(const glm::vec3& scale);
    virtual void setRotation(const glm::vec3 &axis);
    virtual void setName(const std::string& name);

    void setPositionChangedCallback(const std::function<void(const glm::vec3&)>& callback);
    void setTransformationChangedCallback(const std::function<void(const glm::mat4&)>& callback);

    [[nodiscard]] glm::vec3 getPosition() const;
    [[nodiscard]] glm::vec3 getScale() const;
    [[nodiscard]] glm::vec3 getRotation() const;
    [[nodiscard]] const common::LayerMask& getLayerMask() const;
    [[nodiscard]] const std::string& getName() const;
    glm::mat4 getTransformMatrix();
    void setTransformMatrix(const glm::mat4& transformMatrix);

    virtual void destroy();
    virtual void update(float deltaTime);

    template<typename T, typename... Args>
    T* addComponent(Args&&... args)
    {
        const auto type = std::type_index(typeid(T));
        auto comp = std::make_shared<T>(std::forward<Args>(args)...);
        T* ptr = comp.get();
        comp->setOwner(this);
        m_components[type] = std::move(comp);
        return ptr;
    }

    template<typename T>
    T* getComponent()
    {
        const auto it = m_components.find(std::type_index(typeid(T)));
        return it != m_components.end() ? static_cast<T*>(it->second.get()) : nullptr;
    }

    template<typename T>
    bool hasComponent() const
    {
        return m_components.contains(std::type_index(typeid(T)));
    }

    virtual ~GameObject();

    std::unordered_map<int, Material*> overrideMaterials;
private:
    glm::mat4 m_transformMatrix;
    bool m_isTransformMatrixDirty{true};
    std::function<void(const glm::vec3&)> m_positionChangedCallback;
    std::function<void(const glm::mat4&)> m_transformationChangedCallback;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> m_components;
    common::LayerMask m_layerMask{common::LayerMask::DEFAULT};
    glm::vec3 m_position{glm::vec3(0.0f, 0.0f, 0.0f)};
    glm::vec3 m_scale{glm::vec3(1.0f, 1.0f, 1.0f)};
    glm::vec3 m_rotation{0.0f};
    std::string m_name;
};

#endif //GAME_OBJECT_HPP