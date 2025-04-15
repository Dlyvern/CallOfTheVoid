#ifndef SKELETON_HPP
#define SKELETON_HPP

#include "Common.hpp"
#include <unordered_map>
#include <assimp/mesh.h>

class Skeleton
{
public:
    void addBone(aiBone* bone);
    unsigned int addBone(const common::BoneInfo& bone);
    int getBoneId(const std::string& boneName);

    void printBonesHierarchy();

    std::unordered_map<std::string, unsigned int>& getBoneMap() {return m_boneMap;}
    std::vector<common::BoneInfo>& getBones();

    common::BoneInfo* getBone(const std::string& boneName);
    common::BoneInfo* getBone(int boneID);

    void calculateBindPoseTransforms(std::vector<glm::mat4>& outMatrices)
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
        {
            if (bone.parentId == -1)
                processBone(bone.id, identity);
        }
    }


private:
    std::unordered_map<std::string, unsigned int> m_boneMap;
    std::vector<common::BoneInfo> m_bonesInfo;
};


#endif //SKELETON_HPP