#include "StaticModel.hpp"

StaticModel::StaticModel(const std::string &name, const std::vector<StaticMesh> &meshes) : m_name(name), m_meshes(meshes)
{
}

StaticModel::StaticModel() = default;

void StaticModel::draw()
{
    for (auto& mesh : m_meshes)
        mesh.draw();
}
