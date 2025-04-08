#ifndef MESH_HPP
#define MESH_HPP

#include "Common.hpp"

class StaticMesh : public common::Mesh
{
public:
    StaticMesh(const std::vector<common::Vertex>& vertices, const std::vector<unsigned int>& indices);

    void draw() override;
private:
    unsigned int m_vao, m_vbo, m_ebo;
    uint32_t m_indicesCount{0};
};

#endif //MESH_HPP