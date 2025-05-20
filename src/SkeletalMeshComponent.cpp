#include "SkeletalMeshComponent.hpp"

SkeletalMeshComponent::SkeletalMeshComponent(SkinnedModel *model) : m_model(model)
{
    m_model->getSkeleton().calculateBindPoseTransforms();
}

void SkeletalMeshComponent::render(GLitch::Shader &shader, const glm::mat4 &modelMatrix, std::unordered_map<int, Material*> *overrideMaterials) const
{
    shader.setMat4("model", modelMatrix);

    const std::vector<glm::mat4>& boneMatrices = m_model->getSkeleton().getFinalMatrices();

    for (int i = 0; i < boneMatrices.size(); ++i)
        shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", boneMatrices[i]);

    if (overrideMaterials)
        m_model->drawWithMaterials(*overrideMaterials);
    else
        m_model->draw();
}

SkinnedModel* SkeletalMeshComponent::getModel() const
{
    return m_model;
}

void SkeletalMeshComponent::setModel(SkinnedModel *model)
{
    m_model = model;
    m_model->getSkeleton().calculateBindPoseTransforms();
}
