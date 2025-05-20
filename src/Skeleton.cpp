#include "Skeleton.hpp"
#include "Utilities.hpp"
#include <iostream>

Skeleton::Skeleton()
{
    m_finalBoneMatrices.reserve(100);

    for (int i = 0; i < 100; i++)
        m_finalBoneMatrices.emplace_back(1.0f);
}

void Skeleton::addBone(aiBone *bone)
{
    const std::string boneName = bone->mName.C_Str();

    if(!m_boneMap.contains(boneName))
    {
        common::BoneInfo boneInfo;

        boneInfo.name = boneName;
        boneInfo.offsetMatrix = utilities::convertMatrixToGLMFormat(bone->mOffsetMatrix);
        boneInfo.id = m_bonesInfo.size();
        boneInfo.finalTransformation = glm::mat4(1.0f);
        boneInfo.parentId = -1;
        m_boneMap[boneInfo.name] = boneInfo.id;

        m_bonesInfo.push_back(boneInfo);
    }
}

unsigned int Skeleton::addBone(const common::BoneInfo &bone)
{
    if (m_boneMap.contains(bone.name))
        return m_bonesInfo[m_boneMap[bone.name]].id;

    const unsigned int boneID = m_bonesInfo.size();
    m_boneMap[bone.name] = boneID;
    m_bonesInfo.push_back(bone);

    return boneID;
}

int Skeleton::getBoneId(const std::string &boneName)
{
    if (m_boneMap.contains(boneName))
        return m_boneMap[boneName];
    return -1;
}

void Skeleton::printBonesHierarchy()
{
    for (const auto& bone : m_bonesInfo)
    {
        std::cout << "Bone: " << bone.name << " (ID: " << bone.id << "), Parent: "
                  << (bone.parentId == -1 ? "None" : m_bonesInfo[bone.parentId].name)
                  << ", Children: ";
        for (int child : bone.children) {
            std::cout << m_bonesInfo[child].name << " ";
        }
        std::cout << std::endl;
    }
}

size_t Skeleton::getBonesCount() const
{
    return m_bonesInfo.size();
}

bool Skeleton::hasBone(const std::string &boneName) const
{
    return m_boneMap.contains(boneName);
}

common::BoneInfo* Skeleton::getBone(const std::string &boneName)
{
    const int id = getBoneId(boneName);

    if (id == -1)
        return  nullptr;

    return &m_bonesInfo[id];
}

common::BoneInfo* Skeleton::getBone(int boneID)
{
    if (m_bonesInfo.size() < boneID)
        return nullptr;

    return &m_bonesInfo[boneID];
}

common::BoneInfo* Skeleton::getParent()
{
    for (auto& bone : m_bonesInfo)
        if (bone.parentId == -1)
            return &bone;

    return nullptr;
}

void Skeleton::calculateBindPoseTransforms()
{
    m_bindPoseTransform.resize(m_bonesInfo.size(), glm::mat4(1.0f));

    glm::mat4 identity(1.0f);

    auto processBone = [this](int boneID, const glm::mat4 &parentTransform, auto&& self)->void
    {
        common::BoneInfo& bone = m_bonesInfo[boneID];
        glm::mat4 globalTransform = parentTransform * bone.localBindTransform;

        m_bindPoseTransform[boneID] = globalTransform * bone.offsetMatrix;

        bone.finalTransformation = globalTransform * bone.offsetMatrix;

        for (int childID : bone.children)
            self(childID, globalTransform, self);
    };

    for (const auto& bone : m_bonesInfo)
        if (bone.parentId == -1)
            processBone(bone.id, identity, processBone);
}

const std::vector<glm::mat4>& Skeleton::getBindPoses() const
{
    return m_bindPoseTransform;
}

const std::vector<glm::mat4>& Skeleton::getFinalMatrices()
{
    for (const auto& bone : m_bonesInfo)
        m_finalBoneMatrices[bone.id] = bone.finalTransformation;

    return m_finalBoneMatrices;
}
