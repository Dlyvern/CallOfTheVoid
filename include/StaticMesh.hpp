#ifndef MESH_HPP
#define MESH_HPP

#include "Common.hpp"
#include "Material.hpp"

class StaticMesh final : public common::Mesh
{
public:
    StaticMesh(const std::vector<common::Vertex>& vertices, const std::vector<unsigned int>& indices);

    void draw() override;

    void setMaterial(Material* material);

    Material* getMaterial();

    ~StaticMesh() override;
private:
    unsigned int m_vao, m_vbo, m_ebo;
    uint32_t m_indicesCount{0};
    Material* m_material{nullptr};
};

#endif //MESH_HPP