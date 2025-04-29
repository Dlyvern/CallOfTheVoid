#include "SkinnedModel.hpp"

#include <iostream>
#include <ostream>

SkinnedModel::SkinnedModel() = default;

SkinnedModel::SkinnedModel(const std::string &name, const std::vector<SkeletalMesh> &meshes) : Model(name), m_meshes(meshes) {}

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

common::Animation* SkinnedModel::getAnimation(const std::string &name)
{
    for (auto& animation : m_animations)
        if (animation.name == name)
            return &animation;

    std::cout << "SkinnedModel::getAnimation(): Could not fina " << name << " animation" << std::endl;

    return nullptr;
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
