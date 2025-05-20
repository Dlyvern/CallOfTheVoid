#include "SkinnedModel.hpp"

#include <iostream>
#include <ostream>

#include "ShaderManager.hpp"

SkinnedModel::SkinnedModel() = default;

SkinnedModel::SkinnedModel(const std::string &name, const std::vector<SkeletalMesh> &meshes) : Model(name), m_meshes(meshes)
{
    meshes_.reserve(m_meshes.size());

    for (auto &mesh : m_meshes)
        meshes_.push_back(&mesh);
}

void SkinnedModel::setSkeleton(const Skeleton &skeleton)
{
    m_skeleton = skeleton;
}

void SkinnedModel::setAnimations(const std::vector<common::Animation> &animations)
{
    m_animations = animations;
}

void SkinnedModel::drawWithMaterials(std::unordered_map<int, Material*> &materials)
{
    const auto shader = ShaderManager::instance().getShader(ShaderManager::ShaderType::SKELETON);

    for (int meshIndex = 0; meshIndex < m_meshes.size(); meshIndex++)
    {
        auto& mesh = m_meshes[meshIndex];
        Material* material = mesh.getMaterial();

        if (materials.contains(meshIndex))
            material = materials[meshIndex];

        material->bind(*shader);

        mesh.draw();
    }
}

std::vector<common::Animation>& SkinnedModel::getAnimations()
{
    return m_animations;
}

common::Animation* SkinnedModel::getAnimation(const std::string &name)
{
    for (auto& animation : m_animations)
        if (animation.name == name)
            return &animation;

    std::cout << "SkinnedModel::getAnimation(): Could not find " << name << " animation" << std::endl;

    return nullptr;
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
