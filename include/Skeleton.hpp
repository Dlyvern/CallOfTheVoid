#ifndef SKELETON_HPP
#define SKELETON_HPP

#include "Common.hpp"
#include <unordered_map>
#include <assimp/mesh.h>

class Skeleton
{
public:
    Skeleton();
    void addBone(aiBone* bone);
    unsigned int addBone(const common::BoneInfo& bone);
    int getBoneId(const std::string& boneName);

    void printBonesHierarchy();

    size_t getBonesCount() const;

    bool hasBone(const std::string& boneName) const;

    common::BoneInfo* getBone(const std::string& boneName);
    common::BoneInfo* getBone(int boneID);
    common::BoneInfo* getParent();

    void calculateBindPoseTransforms();

    const std::vector<glm::mat4>& getBindPoses() const;

    const std::vector<glm::mat4>& getFinalMatrices();

    glm::mat4 globalInverseTransform;
private:
    bool m_isBaked{false};
    std::vector<glm::mat4> m_bindPoseTransform;
    std::unordered_map<std::string, unsigned int> m_boneMap;
    std::vector<common::BoneInfo> m_bonesInfo;

    std::vector<glm::mat4> m_finalBoneMatrices;
};

#endif //SKELETON_HPP
