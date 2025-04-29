#include "StaticModel.hpp"

StaticModel::StaticModel(const std::string &name, const std::vector<StaticMesh> &meshes) : Model(name), m_meshes(meshes)
{
}

StaticModel::StaticModel() = default;

void StaticModel::draw()
{
    for (auto& mesh : m_meshes)
        mesh.draw();
}

std::vector<StaticMesh>& StaticModel::getMeshes()
{
    return m_meshes;
}
