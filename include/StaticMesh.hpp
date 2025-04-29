#ifndef MESH_HPP
#define MESH_HPP

#include "Common.hpp"
#include "Material.hpp"

class StaticMesh final : public common::Mesh
{
public:
    StaticMesh(const std::vector<common::Vertex>& vertices, const std::vector<unsigned int>& indices);

    StaticMesh();

    void draw() override;

    void setVerticesAndIndices(const std::vector<common::Vertex>& vertices, const std::vector<unsigned int>& indices);

    void loadFromRaw();

    ~StaticMesh() override;
private:
    unsigned int m_vao, m_vbo, m_ebo;
    uint32_t m_indicesCount{0};

    std::vector<common::Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
};

#endif //MESH_HPP