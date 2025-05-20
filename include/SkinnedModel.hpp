#ifndef MODEL_HPP
#define MODEL_HPP

#include "SkeletalMesh.hpp"
#include <string>
#include "Common.hpp"
#include <assimp/scene.h>

#include "Skeleton.hpp"

class SkinnedModel : public common::Model
{
public:
    SkinnedModel();

    explicit SkinnedModel(const std::string& name, const std::vector<SkeletalMesh>& meshes);

    void setSkeleton(const Skeleton& skeleton);

    void setAnimations(const std::vector<common::Animation>& animations);

    void drawWithMaterials(std::unordered_map<int, Material*>& materials);

    std::vector<common::Animation>& getAnimations();

    common::Animation* getAnimation(const std::string& name);

    Skeleton& getSkeleton();

    void draw() override;
private:
    std::vector<SkeletalMesh> m_meshes;
    std::vector<common::Animation> m_animations;
    Skeleton m_skeleton;
};


#endif //MODEL_HPP