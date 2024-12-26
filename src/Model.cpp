#include "Model.hpp"

#include "../libraries/glad/glad.h"

Model::Model() = default;

Model::Model(const std::string &name, const std::vector<Mesh> &meshes) : m_meshes(meshes), m_name(name) {}

void Model::setBoneMap(const std::unordered_map<std::string, unsigned int> &boneMap)
{
    m_boneMap = boneMap;
}

void Model::setBonesInfo(const std::vector<common::BoneInfo> &bonesInfo)
{
    m_bonesInfo = bonesInfo;
    boneTransforms.resize(m_bonesInfo.size(), glm::mat4(1.0f));
}

void Model::setVertexBoneData(const std::vector<common::VertexBoneData> &vertexBoneData)
{
    m_vertexBoneData = vertexBoneData;
}

void Model::setAnimations(const std::vector<Animation> &animations)
{
    m_animations = animations;
}

std::unordered_map<std::string, unsigned int> Model::getBoneMap() const
{
    return m_boneMap;
}

std::vector<common::BoneInfo> Model::getBonesInfo() const
{
    return m_bonesInfo;
}

std::vector<Animation> Model::getAnimations()
{
    return m_animations;
}

const std::string &Model::getName() const
{
    return m_name;
}

void Model::draw()
{
    for (auto& mesh : m_meshes)
        mesh.draw();
}
