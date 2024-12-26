#ifndef MESH_HPP
#define MESH_HPP

#include "Common.hpp"
#include <vector>

class Mesh
{
public:
    Mesh(const std::vector<common::Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<common::VertexBoneData>& vertexBoneData);

    void draw();

private:
    unsigned int m_vao, m_vbo, m_ebo;
    std::vector<common::Vertex> m_vertices;
    uint32_t m_indicesCount{0};
};

#endif //MESH_HPP