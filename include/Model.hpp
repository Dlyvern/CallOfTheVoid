#ifndef MODEL_HPP
#define MODEL_HPP

#include "Mesh.hpp"
#include <string>
#include <unordered_map>
#include "Animation.hpp"

class Model
{
public:
    Model();
    
    explicit Model(const std::string& name, const std::vector<Mesh>& meshes);

    void setBoneMap(const std::unordered_map<std::string, unsigned int>& boneMap);

    void setBonesInfo(const std::vector<common::BoneInfo>& bonesInfo);

    void setVertexBoneData(const std::vector<common::VertexBoneData>& vertexBoneData);

    void setAnimations(const std::vector<Animation>& animations);

    std::unordered_map<std::string, unsigned int> getBoneMap() const;
    std::vector<common::BoneInfo> getBonesInfo() const;

    std::vector<Animation> getAnimations();

    [[nodiscard]] const std::string& getName() const;

    void draw();

    std::vector<glm::mat4> boneTransforms;

private:
    std::vector<Mesh> m_meshes;
    std::string m_name{"undefined"};
    std::unordered_map<std::string, unsigned int> m_boneMap;
    std::vector<common::BoneInfo> m_bonesInfo;
    std::vector<Animation> m_animations;
    std::vector<common::VertexBoneData> m_vertexBoneData;
};


#endif //MODEL_HPP