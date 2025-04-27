#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include <memory>
#include <typeindex>
#include <unordered_map>

#include "Common.hpp"
#include "Shader.hpp"
#include "Component.hpp"
#include "Material.hpp"

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
    virtual void calculateShadows(GLitch::Shader &shader) = 0;

    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        auto type = std::type_index(typeid(T));
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = comp.get();
        comp->setOwner(this);
        m_components[type] = std::move(comp);
        return ptr;
    }

    template<typename T>
    T* getComponent() {
        auto it = m_components.find(std::type_index(typeid(T)));
        return it != m_components.end() ? static_cast<T*>(it->second.get()) : nullptr;
    }

    template<typename T>
    bool hasComponent() const {
        return m_components.contains(std::type_index(typeid(T)));
    }

    virtual void destroy() {}

    Material& getMaterial();
    
    void setMaterial(const Material& material);
    void setMaterial(Material* material);

    virtual ~GameObject();

protected:
    void updateComponents(float deltaTime)
    {
        for (auto& [_, comp] : m_components)
            comp->update(deltaTime);
    }
private:
    Material m_material;
    std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components;
    common::LayerMask m_layerMask{common::LayerMask::DEFAULT};
    glm::vec3 m_position{glm::vec3(0.0f, 0.0f, 0.0f)};
    glm::vec3 m_scale{glm::vec3(1.0f, 1.0f, 1.0f)};
    glm::vec3 m_rotation{0.0f};
    common::AABB m_boundingBox;
    std::string m_name;
};

#endif //GAME_OBJECT_HPP