#include "StaticModel.hpp"

#include "ShaderManager.hpp"

StaticModel::StaticModel(const std::string &name, const std::vector<StaticMesh> &meshes) : Model(name), m_meshes(meshes)
{
    meshes_.reserve(m_meshes.size());

    for (auto &mesh : m_meshes)
        meshes_.push_back(&mesh);
}

StaticModel::StaticModel() = default;

void StaticModel::draw()
{
    for (auto& mesh : m_meshes)
        mesh.draw();
}

void StaticModel::drawWithMaterials(std::unordered_map<int, Material*> &materials)
{
    const auto shader = ShaderManager::instance().getShader(ShaderManager::ShaderType::STATIC);

    for (int meshIndex = 0; meshIndex < m_meshes.size(); meshIndex++)
    {
        auto& mesh = m_meshes[meshIndex];
        Material* material = mesh.getMaterial();

        if (materials.contains(meshIndex))
            material = materials[meshIndex];

        material->bind(*shader);

        mesh.draw();
    }
}