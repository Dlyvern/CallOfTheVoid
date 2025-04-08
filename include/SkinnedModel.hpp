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

    std::vector<common::Animation>& getAnimations();

    [[nodiscard]] const std::string& getName() const;

    Skeleton& getSkeleton();

    void draw() override;
private:
    std::vector<SkeletalMesh> m_meshes;
    std::string m_name{"undefined"};
    std::vector<common::Animation> m_animations;
    Skeleton m_skeleton;
};


#endif //MODEL_HPP