#include "Skeleton.hpp"

#include "Utilities.hpp"
#include <iostream>

void Skeleton::addBone(aiBone *bone)
{
    std::string boneName = bone->mName.C_Str();

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

std::vector<common::BoneInfo> & Skeleton::getBones()
{
    return m_bonesInfo;
}

common::BoneInfo* Skeleton::getBone(const std::string &boneName)
{
    int id = getBoneId(boneName);

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

void Skeleton::calculateRagdollBoneMatrices(std::vector<glm::mat4> &outMatrices) const
{
    outMatrices.resize(m_bonesInfo.size(), glm::mat4(1.0f));

    for (int i = 0; i < m_bonesInfo.size(); ++i)
    {
        const auto& bone = m_bonesInfo[i];

        if (bone.rigidBody)
        {
            physx::PxTransform t = bone.rigidBody->getGlobalPose();
            glm::quat rotQuat(t.q.w, t.q.x, t.q.y, t.q.z);
            glm::mat4 rot = glm::toMat4(rotQuat);
            glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(t.p.x, t.p.y, t.p.z));
            glm::mat4 physTransform = trans * rot;
            glm::mat4 finalTransform = globalInverseTransform * physTransform * bone.offsetMatrix;

            outMatrices[i] = finalTransform;
        }
        else
            outMatrices[i] = glm::mat4(1.0f);
    }
}

void Skeleton::calculateBindPoseTransforms(std::vector<glm::mat4> &outMatrices)
{
    outMatrices.resize(m_bonesInfo.size(), glm::mat4(1.0f));

    glm::mat4 identity(1.0f);

    std::function<void(int, glm::mat4)> processBone;
    processBone = [&](int boneID, glm::mat4 parentTransform)
    {
        common::BoneInfo& bone = m_bonesInfo[boneID];
        glm::mat4 globalTransform = parentTransform * bone.localBindTransform;

        outMatrices[boneID] = globalTransform * bone.offsetMatrix;

        for (int childID : bone.children)
            processBone(childID, globalTransform);
    };

    for (const auto& bone : m_bonesInfo)
        if (bone.parentId == -1)
            processBone(bone.id, identity);
}
