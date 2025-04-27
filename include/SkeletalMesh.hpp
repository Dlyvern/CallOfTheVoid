#ifndef SKELETAL_MESH_HPP
#define SKELETAL_MESH_HPP
#include "Common.hpp"
#include "Material.hpp"

class SkeletalMesh final : public common::Mesh
{
public:
    SkeletalMesh(const std::vector<common::Vertex>& vertices, const std::vector<unsigned int>& indices);

    void draw() override;

    void setMaterial(Material* material);
    Material* getMaterial() const;
private:
    unsigned int m_vao, m_vbo, m_ebo;
    std::vector<common::Vertex> m_vertices;
    uint32_t m_indicesCount{0};

    Material* m_material{nullptr};
};

#endif //SKELETAL_MESH_HPP
