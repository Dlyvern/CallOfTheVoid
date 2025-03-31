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

unsigned int Skeleton::getBoneId(const std::string &boneName)
{
    return m_boneMap[boneName];
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
