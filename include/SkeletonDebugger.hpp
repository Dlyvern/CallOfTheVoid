#ifndef SKELETONDEBUGGER_HPP
#define SKELETONDEBUGGER_HPP
#include <vector>
#include "Common.hpp"



class SkeletonDebugger {
public:
    SkeletonDebugger();
    ~SkeletonDebugger();

    void Init();
    void Draw(const std::vector<common::BoneInfo>& bones);

private:
    unsigned int VAO, VBO;
    void UploadSkeletonData(const std::vector<std::pair<glm::vec3, glm::vec3>>& boneLines);
    std::vector<std::pair<glm::vec3, glm::vec3>> GetBoneDebugLines(const std::vector<common::BoneInfo>& bones);
};
#endif //SKELETONDEBUGGER_HPP
