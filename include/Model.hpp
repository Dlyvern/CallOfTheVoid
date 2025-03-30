#ifndef MODEL_HPP
#define MODEL_HPP

#include "Mesh.hpp"
#include <string>
#include <unordered_map>
#include "Common.hpp"
#include <assimp/scene.h>

class Model
{
public:
    Model();

    explicit Model(const std::string& name, const std::vector<Mesh>& meshes);

    void setBoneMap(const std::unordered_map<std::string, unsigned int>& boneMap);

    void setBonesInfo(const std::vector<common::BoneInfo>& bonesInfo);

    void setAnimations(const std::vector<common::Animation>& animations);

    std::unordered_map<std::string, unsigned int> getBoneMap() const;
    std::vector<common::BoneInfo>& getBonesInfo();

    std::vector<common::Animation>& getAnimations();

    [[nodiscard]] const std::string& getName() const;

    void draw();

    std::vector<glm::mat4> boneTransforms;

    aiNode* rootNode{nullptr};
    glm::mat4 globalInverseTransform;

private:
    std::vector<Mesh> m_meshes;
    std::string m_name{"undefined"};
    std::unordered_map<std::string, unsigned int> m_boneMap;
    std::vector<common::BoneInfo> m_bonesInfo;
    std::vector<common::Animation> m_animations;
};


#endif //MODEL_HPP