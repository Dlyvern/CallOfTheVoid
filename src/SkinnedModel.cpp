#include "SkinnedModel.hpp"

SkinnedModel::SkinnedModel() = default;

SkinnedModel::SkinnedModel(const std::string &name, const std::vector<SkeletalMesh> &meshes) : m_meshes(meshes), m_name(name) {}

void SkinnedModel::setSkeleton(const Skeleton &skeleton)
{
    m_skeleton = skeleton;
}

void SkinnedModel::setAnimations(const std::vector<common::Animation> &animations)
{
    m_animations = animations;
}

std::vector<common::Animation>& SkinnedModel::getAnimations()
{
    return m_animations;
}

const std::string &SkinnedModel::getName() const
{
    return m_name;
}

std::vector<SkeletalMesh>& SkinnedModel::getMeshes()
{
    return m_meshes;
}

Skeleton& SkinnedModel::getSkeleton()
{
    return m_skeleton;
}

void SkinnedModel::draw()
{
    for (auto& mesh : m_meshes)
        mesh.draw();
}
