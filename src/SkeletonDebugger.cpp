#include "SkeletonDebugger.hpp"

#include <glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

SkeletonDebugger::SkeletonDebugger() : VAO(0), VBO(0) {}

SkeletonDebugger::~SkeletonDebugger() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void SkeletonDebugger::Init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

glm::vec3 GetBonePosition(const glm::mat4& transform) {
    return {transform[3]}; // Extract translation component
}

std::vector<std::pair<glm::vec3, glm::vec3>> SkeletonDebugger::GetBoneDebugLines(const std::vector<common::BoneInfo>& bones) {
    std::vector<std::pair<glm::vec3, glm::vec3>> boneLines;
    for (const auto& bone : bones)
    {
        glm::vec3 parentPos = GetBonePosition(bone.finalTransformation);

        for (const auto& child : bone.children)
        {
            for (const auto& b : bones)
                if (b.id == child)
                {
                    glm::vec3 childPos = GetBonePosition(b.finalTransformation);
                    boneLines.emplace_back(parentPos, childPos);

                    std::cout << "  -> Child: " << b.name << " Position: " << childPos.x << ", " << childPos.y << ", " << childPos.z << std::endl;
                }
        }
    }
    return boneLines;
}

void SkeletonDebugger::UploadSkeletonData(const std::vector<std::pair<glm::vec3, glm::vec3>>& boneLines) {
    std::vector<glm::vec3> lineVertices;
    for (const auto& line : boneLines) {
        lineVertices.push_back(line.first);
        lineVertices.push_back(line.second);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(glm::vec3), lineVertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
}

void SkeletonDebugger::Draw(const std::vector<common::BoneInfo>& bones) {
    auto boneLines = GetBoneDebugLines(bones);
    UploadSkeletonData(boneLines);

    glDisable(GL_DEPTH_TEST); // Prevent depth buffer from hiding lines
    glLineWidth(10.0f); // Make debug lines thicker
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, boneLines.size() * 2);
    glEnable(GL_DEPTH_TEST); // Re-enable depth test
}