#include "StaticMeshComponent.hpp"

StaticMeshComponent::StaticMeshComponent(StaticModel *model) : m_model(model)
{
}

void StaticMeshComponent::render(GLitch::Shader &shader, const glm::mat4 &modelMatrix, std::unordered_map<int, Material *> *overrideMaterials) const
{
    shader.setMat4("model", modelMatrix);

    if (overrideMaterials)
        m_model->drawWithMaterials(*overrideMaterials);
    else
        m_model->draw();
}

StaticModel* StaticMeshComponent::getModel() const
{
    return m_model;
}

void StaticMeshComponent::setModel(StaticModel *model)
{
    m_model = model;
}
