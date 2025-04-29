#include "SkeletalMesh.hpp"
#include "glad.h"
#include "ShaderManager.hpp"

SkeletalMesh::SkeletalMesh(const std::vector<common::Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    setVerticesAndIndices(vertices, indices);
    loadFromRaw();
}

SkeletalMesh::SkeletalMesh() = default;

void SkeletalMesh::setVerticesAndIndices(const std::vector<common::Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    m_vertices = vertices;
    m_indices = indices;
}

void SkeletalMesh::loadFromRaw()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(common::Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(common::Vertex), (void*)offsetof(common::Vertex, position));
    glEnableVertexAttribArray(0);

    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(common::Vertex), (void*)offsetof(common::Vertex, normal));
    glEnableVertexAttribArray(1);

    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(common::Vertex), (void*)offsetof(common::Vertex, textureCoordinates));
    glEnableVertexAttribArray(2);

    // Tangents
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(common::Vertex), (void*)offsetof(common::Vertex, tangent));
    glEnableVertexAttribArray(3);

    // Bitangents
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(common::Vertex), (void*)offsetof(common::Vertex, bitangent));
    glEnableVertexAttribArray(4);

    // Bone IDs
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(common::Vertex), (void*)offsetof(common::Vertex, boneID));
    glEnableVertexAttribArray(5);

    // Weights
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(common::Vertex), (void*)offsetof(common::Vertex, weight));
    glEnableVertexAttribArray(6);


    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_indicesCount = m_indices.size();
}

void SkeletalMesh::draw()
{
    // if (getMaterial())
    // {
    //     auto shader = ShaderManager::instance().getShader(ShaderManager::ShaderType::SKELETON);
    //     getMaterial()->bind(*shader);
    // }
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
