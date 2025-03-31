#ifndef SKELETAL_MESH_HPP
#define SKELETAL_MESH_HPP
#include "Common.hpp"

class SkeletalMesh
{
public:
    SkeletalMesh(const std::vector<common::Vertex>& vertices, const std::vector<unsigned int>& indices);

    void draw();
private:
    unsigned int m_vao, m_vbo, m_ebo;
    std::vector<common::Vertex> m_vertices;
    uint32_t m_indicesCount{0};
};

#endif //SKELETAL_MESH_HPP
