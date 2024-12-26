#ifndef COMMON_HPP
#define COMMON_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace common
{
    enum LayerMask
    {
        DEFAULT,
        GROUND,
        PLAYER,
    };

    struct Vertex
    {   
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 textureCoordinates;
        glm::vec3 tangent;
        glm::vec3 bitangent;
        glm::ivec4 boneID = glm::ivec4(0);
        glm::vec4 weight = glm::vec4(0);
    };

    struct VertexBoneData
    {
        glm::vec3 position = glm::vec3(0);
        glm::vec3 normal = glm::vec3(0);
        glm::vec2 uv = glm::vec2(0);
        glm::vec3 tangent = glm::vec3(0);
        glm::ivec4 boneID = glm::ivec4(0);
        glm::vec4 weight = glm::vec4(0);
    };

    struct BoneInfo
    {
        std::string name;
        int id;
        glm::mat4 offsetMatrix;
        glm::mat4 finalTransformation;
    };

    struct SQT
    {
        glm::quat rotation = glm::quat(1, 0, 0, 0);
        glm::vec3 position = glm::vec3(0, 0, 0);
        float scale = 1.0f;
        float timeStamp = -1;
        std::string jointName;
    };

    struct BoneAnimation
    {
        std::vector<SQT> keyFrames;
    };

    struct AABB
    {
        glm::vec3 min;
        glm::vec3 max;
    };
} //namespace common


#endif //COMMON_HPP