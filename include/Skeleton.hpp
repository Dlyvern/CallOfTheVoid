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

    common::BoneInfo* getParent();

    void calculateRagdollBoneMatrices(std::vector<glm::mat4>& outMatrices) const;

    void calculateBindPoseTransforms(std::vector<glm::mat4>& outMatrices);

    glm::mat4 globalInverseTransform;
private:
    std::unordered_map<std::string, unsigned int> m_boneMap;
    std::vector<common::BoneInfo> m_bonesInfo;
};

#endif //SKELETON_HPP
