#ifndef STATIC_MODEL_HPP
#define STATIC_MODEL_HPP
#include "StaticMesh.hpp"

class StaticModel final : public common::Model
{
public:
    StaticModel(const std::string& name, const std::vector<StaticMesh>& meshes);

    StaticModel();

    void draw() override;

    void drawWithMaterials(std::unordered_map<int, Material*>& materials);
private:
    std::vector<StaticMesh> m_meshes;
};

#endif //STATIC_MODEL_HPP
