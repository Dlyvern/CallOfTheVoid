#ifndef SKELETAL_MESH_COMPONENT_HPP
#define SKELETAL_MESH_COMPONENT_HPP

#include "Component.hpp"
#include "SkinnedModel.hpp"

class SkeletalMeshComponent final : public Component
{
public:
    explicit SkeletalMeshComponent(SkinnedModel* model);

    void update(float deltaTime) override {}

    void render(GLitch::Shader& shader, const glm::mat4& modelMatrix, std::unordered_map<int, Material *>* overrideMaterials = nullptr) const;

    [[nodiscard]] SkinnedModel* getModel() const;

    void setModel(SkinnedModel* model);
private:
    SkinnedModel* m_model{nullptr};
};

#endif //SKELETAL_MESH_COMPONENT_HPP
