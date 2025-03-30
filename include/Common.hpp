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
        glm::ivec4 boneID = glm::ivec4(-1);
        glm::vec4 weight = glm::vec4(0);
    };

    struct BoneInfo
    {
        std::string name;
        int id;
        glm::mat4 offsetMatrix;
        glm::mat4 finalTransformation;
        std::vector<int> children;
        std::vector<BoneInfo*> childrenInfo;
        int parentId{-1};
    };

    struct BoneDebug {
        glm::vec3 position;
        std::vector<BoneDebug*> children;
    };

    struct SQT
    {
        glm::quat rotation = glm::quat(1, 0, 0, 0);
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 scale = glm::vec3(0, 0, 0);
        float timeStamp = -1;
        std::string jointName;
    };

    struct BoneAnimation
    {
        std::vector<SQT> keyFrames;
        glm::mat4 transform{1.0f};
        std::string boneName;
    };

    struct Animation
    {
        std::string name;
        double ticksPerSecond;
        double duration;
        std::vector<BoneAnimation> boneAnimations;
        //MAYBE NOT VERY GOOD IDEA
        std::vector<common::BoneInfo>* modelBones{nullptr};

        common::BoneAnimation* getBoneAnimation(const std::string& name)
        {
            for (auto& bone : boneAnimations)
                if (bone.boneName == name)
                    return &bone;

            return nullptr;
        }

        common::BoneInfo* findParent()
        {
            for (int i = 0; i < modelBones->size(); ++i)
                if (modelBones->at(i).parentId == -1)
                    return &modelBones->at(i);
            return nullptr;
        }

        common::BoneInfo* findBone(int index)
        {
            return &modelBones->at(index);
        }

        common::BoneInfo* findBone(const std::string& name) {
            for (auto& bone : *modelBones)
            {
                if (bone.name == name)
                    return &bone;
            }

            return nullptr;
        }
    };

    struct AABB
    {
        glm::vec3 min;
        glm::vec3 max;
    };
} //namespace common


#endif //COMMON_HPP