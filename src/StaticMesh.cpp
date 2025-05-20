#include "StaticMesh.hpp"
#include "glad.h"
#include "ShaderManager.hpp"

StaticMesh::StaticMesh(const std::vector<common::Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    setVerticesAndIndices(vertices, indices);
    loadFromRaw();
}

StaticMesh::StaticMesh() = default;

void StaticMesh::draw()
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void StaticMesh::setVerticesAndIndices(const std::vector<common::Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    m_vertices = vertices;
    m_indices = indices;
}

void StaticMesh::loadFromRaw()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);


    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(common::Vertex), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(common::Vertex), (void*)offsetof(common::Vertex, position));
    glEnableVertexAttribArray(0);

    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(common::Vertex), (void*)offsetof(common::Vertex, normal));
    glEnableVertexAttribArray(1);

    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(common::Vertex), (void*)offsetof(common::Vertex, textureCoordinates));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_indicesCount = m_indices.size();
}

StaticMesh::~StaticMesh()
{
    // if (m_ebo) glDeleteBuffers(1, &m_ebo);
    // if (m_vbo) glDeleteBuffers(1, &m_vbo);
    // if (m_vao) glDeleteVertexArrays(1, &m_vao);
}
