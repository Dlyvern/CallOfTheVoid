#ifndef COMMON_HPP
#define COMMON_HPP

#include <PxRigidDynamic.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "Material.hpp"

namespace common
{
    //Maybe 32 is way too big
    enum LayerMask : uint32_t
    {
        DEFAULT = 1 << 0,
        GROUND = 1 << 1,
        PLAYER = 1 << 2,
        INTERACTABLE = 1 << 3,
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
        glm::mat4 localBindTransform{1.0f};
        glm::mat4 globalBindTransform{1.0f};
        std::vector<int> children;
        std::vector<BoneInfo*> childrenInfo;
        int parentId{-1};
        physx::PxRigidDynamic* rigidBody = nullptr;
    };

    class Mesh
    {
    public:
        virtual void draw() = 0;
        virtual ~Mesh() = default;

        void setMaterial(Material* material) { m_material = material; }

        Material* getMaterial() { return m_material; }
    private:
        Material* m_material{nullptr};
    };

    class Model
    {
    public:
        enum class ModelType : uint8_t
        {
            SKINNED = 0,
            STATIC = 1
        };

        explicit Model(const std::string& name) : m_name(name){}

        Model() = default;

        virtual ~Model() = default;

        [[nodiscard]] std::string getName() const { return m_name; }

        virtual Mesh* getMesh(int index) = 0;

        virtual size_t getMeshesSize() = 0;

        virtual ModelType getType() = 0;

        virtual void draw() = 0;

    private:
        std::string m_name;
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
            for (auto& modelBone : *modelBones)
                if (modelBone.parentId == -1)
                    return &modelBone;

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