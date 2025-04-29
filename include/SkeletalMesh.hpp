#ifndef SKELETAL_MESH_HPP
#define SKELETAL_MESH_HPP
#include "Common.hpp"
#include "Material.hpp"

class SkeletalMesh final : public common::Mesh
{
public:
    SkeletalMesh(const std::vector<common::Vertex>& vertices, const std::vector<unsigned int>& indices);
    SkeletalMesh();

    void setVerticesAndIndices(const std::vector<common::Vertex>& vertices, const std::vector<unsigned int>& indices);

    void loadFromRaw();

    void draw() override;
private:
    unsigned int m_vao, m_vbo, m_ebo;
    uint32_t m_indicesCount{0};
    std::vector<common::Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
};

#endif //SKELETAL_MESH_HPP
