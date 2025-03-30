#include "AssetsManager.hpp"

#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Common.hpp"

#include "Filesystem.hpp"

#include <unordered_map>
#include <map>

//TODO: FIX THAT SHIT
namespace
{
	glm::mat4 convertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
        glm::mat4 to;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }

    glm::vec3 getGLMVec(const aiVector3D& vec)
	{ 
		return {vec.x, vec.y, vec.z};
	}

	glm::quat getGLMQuat(const aiQuaternion& pOrientation)
	{
		return {pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z};
	}
}

AssetsManager& AssetsManager::instance()
{
    static AssetsManager assetsManager;
    return assetsManager;
}

//  void LoadAnimations(const aiScene* scene, const std::string& animPath) {
//         Assimp::Importer importer;
//         const aiScene* animScene = importer.ReadFile(animPath, aiProcess_Triangulate | aiProcess_FlipUVs);
//         if (!animScene || animScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !animScene->mRootNode) {
//             std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
//             return;
//         }

//         animations = animScene->mAnimations;
//         // Process animations here
//     }

void AssetsManager::initMinimum()
{
    auto texturesFolderPath = filesystem::getTexturesFolderPath();
    auto modelsFolderPath = filesystem::getModelsFolderPath();

    if(!std::filesystem::exists(texturesFolderPath))
        throw std::runtime_error("AssetsManager::initMinimum(): Texture folder does not exist");

    if(!std::filesystem::exists(modelsFolderPath))
        throw std::runtime_error("AssetsManager::initMinimum(): Models folder does not exist");

    const textures::Texture textureVoid(std::filesystem::path(texturesFolderPath.string() + "/void.png").string());
    m_textures[textureVoid.getName()] = textureVoid;

    const auto model = loadModel(modelsFolderPath.string() + "/void.fbx");
    m_models[model.getName()] = model;
}

void AssetsManager::loadTextures()
{
    const auto texturesFolderPath = filesystem::getTexturesFolderPath();

    if(!std::filesystem::exists(texturesFolderPath))
        throw std::runtime_error("AssetsManager::loadTextures(): Texture folder does not exist");

    for(const auto& entry : std::filesystem::directory_iterator(texturesFolderPath))
    {
        if(entry.path().extension() == ".png")
        {
            textures::Texture texture{entry.path()};

            if(!m_textures.contains(texture.getName()))
                m_textures[texture.getName()] = texture;
            // texture.bake();
        }
    }
}

void AssetsManager::loadModels()
{
    // std::cout << "Loading models" << std::endl;

    auto modelsFolderPath = filesystem::getModelsFolderPath();

    if(!std::filesystem::exists(modelsFolderPath))
        throw std::runtime_error("AssetsManager::loadModels(): Models folder does not exist");

    for(const auto& entry : std::filesystem::directory_iterator(modelsFolderPath))
    {
        if(entry.path().extension() == ".obj" || entry.path().extension() == ".fbx")
        {
            std::filesystem::path file(entry.path());
    
            std::string name = file.filename().string();

            if(m_models.find(name) != m_models.end())
            {
                // std::cout << "Already exists " << name << std::endl;
                continue;
            }

            auto model = loadModel(entry.path());            
            m_models[name] = model;
            // std::cout << "Added " << name << std::endl;

        }
    }
}

//TODO FIX IT LATER

void extractBoneHierarchy(aiNode* node, int parentID, std::unordered_map<std::string, unsigned int>& bonesMap,  std::vector<common::BoneInfo>& bones)
{
    std::string nodeName = node->mName.C_Str();

    auto it = bonesMap.find(nodeName);
    int currentID = (it != bonesMap.end()) ? it->second : -1;

    if (currentID != -1) {
        bones[currentID].parentId = parentID;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        auto childNode = node->mChildren[i];
        std::string childName = childNode->mName.C_Str();

        auto childIt = bonesMap.find(childName);

        if (childIt != bonesMap.end()) {
            int childID = childIt->second;

            // ✅ Only add the child once (avoid duplicates)
            if (currentID != -1) {
                auto& parentBone = bones[currentID];
                if (std::find(parentBone.children.begin(), parentBone.children.end(), childID) == parentBone.children.end()) {
                    bones[currentID].children.push_back(childID);
                }
                // Recursively process the child
                extractBoneHierarchy(node->mChildren[i], currentID, bonesMap, bones);
            }
        } else {
            // Even if this node isn't a bone, continue processing
            extractBoneHierarchy(node->mChildren[i], currentID, bonesMap, bones);
        }
    }
}

void generateBoneHierarchy(common::BoneInfo* parent, const aiNode* src, std::unordered_map<std::string, unsigned int>& bonesMap, std::vector<common::BoneInfo>& bones)
{
    auto aiMatrix4x4ToGlm = [](const aiMatrix4x4* from) -> glm::mat4
    {
        glm::mat4 to;
        to[0][0] = from->a1; to[1][0] = from->a2; to[2][0] = from->a3; to[3][0] = from->a4;
        to[0][1] = from->b1; to[1][1] = from->b2; to[2][1] = from->b3; to[3][1] = from->b4;
        to[0][2] = from->c1; to[1][2] = from->c2; to[2][2] = from->c3; to[3][2] = from->c4;
        to[0][3] = from->d1; to[1][3] = from->d2; to[2][3] = from->d3; to[3][3] = from->d4;
        return to;
    };

    assert(src);

    std::string nodeName = src->mName.C_Str();
    // Clean up child name to avoid duplicates due to naming conventions
    size_t underscorePos = nodeName.find_first_of('_');
    if (underscorePos != std::string::npos)
        nodeName = nodeName.substr(0, underscorePos);

    int boneID = -1;

    if (bonesMap.contains(nodeName))
    {
        boneID = bonesMap[nodeName];
    }
    else
    {
        // If the bone isn't in the bonesMap, create a new one to preserve hierarchy
        boneID = bones.size();
        bonesMap[nodeName] = boneID;
        auto& ref = bones.emplace_back(common::BoneInfo{nodeName, boneID, glm::mat4(1.0f), glm::mat4(1.0f)});
        ref.offsetMatrix = aiMatrix4x4ToGlm(&src->mTransformation);
    }

    common::BoneInfo& currentBone = bones[boneID];
    // currentBone.offsetMatrix = aiMatrix4x4ToGlm(&src->mTransformation);

    if (parent && parent->id != boneID)
    {
        currentBone.parentId = parent->id;
        parent->children.push_back(boneID);
        parent->childrenInfo.push_back(&currentBone);
    }

    for (unsigned int i = 0; i < src->mNumChildren; i++)
    {
        generateBoneHierarchy(&currentBone, src->mChildren[i], bonesMap, bones);
    }
}

Mesh processMesh(aiMesh* mesh, const aiScene* scene, std::unordered_map<std::string, unsigned int>& bonesMap, std::vector<common::BoneInfo>& bones, std::vector<common::Animation>&animations, glm::mat4& globalInverseTransform)
{
    std::vector<common::Vertex> vertices;
    std::vector<unsigned int> indices;

    for(unsigned int j = 0; j < mesh->mNumVertices; j++)
    {
        common::Vertex vertex;

        for (int i = 0; i < 4; i++)
		{
			vertex.boneID[i] = -1;
			vertex.weight[i] = 0.0f;
		}

        vertex.position = {mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z};
        vertex.normal = {mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z};

        if(mesh->HasTangentsAndBitangents())
        {
            vertex.tangent = {mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z};
            vertex.bitangent = {mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z};
        }
        else
        {
            vertex.tangent = {0.0f, 0.0f, 0.0f};
            vertex.bitangent = {0.0f, 0.0f, 0.0f};
        }

        if(mesh->HasTextureCoords(0))
            vertex.textureCoordinates = {mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y};
        else
            vertex.textureCoordinates = {0.0f, 0.0f};

        vertices.push_back(vertex);
    }

    for(unsigned int k = 0; k < mesh->mNumFaces; k++)
    {
        aiFace face = mesh->mFaces[k];

        for(unsigned int s = 0; s < face.mNumIndices; s++)
            indices.push_back(face.mIndices[s]);
    }

    globalInverseTransform = convertMatrixToGLMFormat(scene->mRootNode->mTransformation);

    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
    {
        aiBone* bone = mesh->mBones[boneIndex];
        std::string boneName = bone->mName.C_Str();

        if(!bonesMap.contains(boneName))
        {
            common::BoneInfo boneInfo;

            boneInfo.name = boneName;
            boneInfo.offsetMatrix = convertMatrixToGLMFormat(bone->mOffsetMatrix);
            boneInfo.id = bones.size();
            boneInfo.finalTransformation = glm::mat4(1.0f);
            boneInfo.parentId = -1;
            bonesMap[boneInfo.name] = boneInfo.id;

            bones.push_back(boneInfo);
        }

        unsigned int boneId = bonesMap[boneName];

        for (unsigned int bonesWeightIndex = 0; bonesWeightIndex < bone->mNumWeights; bonesWeightIndex++)
        {
            auto boneWeight = bone->mWeights[bonesWeightIndex];

            unsigned int vertexId = boneWeight.mVertexId;
            float weight = boneWeight.mWeight;

            common::Vertex& vertexBoneData = vertices[vertexId];

            for(int f = 0; f < 4; ++f)
            {
                if(vertexBoneData.boneID[f] < 0)
                {
                    vertexBoneData.weight[f] = weight;
                    vertexBoneData.boneID[f] = boneId;
                    break;
                }
            }
        }
    }

    generateBoneHierarchy(nullptr, scene->mRootNode, bonesMap, bones);

    for (auto& boneData : bones)
    {
        if (aiNode* boneNode = scene->mRootNode->FindNode(boneData.name.c_str()))
        {
            for (unsigned int childIndex = 0; childIndex < boneNode->mNumChildren; childIndex++)
            {
                std::string childName = boneNode->mChildren[childIndex]->mName.C_Str();

                //TODO: REALLY FUCKING WEIRD SHIT I HATE IT PLEASE KILL ME
                size_t shit = childName.find_first_of('_');

                if (shit != std::string::npos)
                    childName = childName.substr(0, shit);

                if (bonesMap.contains(childName) && childName != boneNode->mName.C_Str())
                {
                    int childID = bonesMap[childName];

                    if (std::ranges::find_if(bones[boneData.id].children, [&childID](int child) {  return child == childID; }) == bones[boneData.id].children.end())
                    {
                        bones[boneData.id].children.push_back(childID);
                        bones[boneData.id].childrenInfo.push_back(&bones[childID]);
                    }

                }
            }
        }
    }

    // //TODO THIS CODE IS VERYYYYYYYYYYYYYYY SLOW
    for (auto& parentBone : bones)
        for (int childID : parentBone.children)
            if (bones[childID].parentId == -1)
                bones[childID].parentId = parentBone.id;



    for(unsigned int i = 0; i < scene->mNumAnimations; ++i)
    {
        aiAnimation* anim = scene->mAnimations[i];
        common::Animation animation;

        animation.name = anim->mName.C_Str();
        animation.duration = anim->mDuration;
        animation.ticksPerSecond = anim->mTicksPerSecond;
        std::vector<common::BoneAnimation> boneAnimations;

        for(unsigned int animChannelIndex = 0; animChannelIndex < anim->mNumChannels; ++animChannelIndex)
        {
            aiNodeAnim* channel = anim->mChannels[animChannelIndex];
            common::BoneAnimation boneAnimation;

            boneAnimation.boneName = channel->mNodeName.C_Str();

            //Maybe we do not need that but some bones from animations do not load up into model bones
            bool cool = false;
            for (const auto& b : bones)
                if (b.name == boneAnimation.boneName)
                    cool = true;

            if (!cool) {
                common::BoneInfo tempBoneInfo;
                tempBoneInfo.name = boneAnimation.boneName;
                bones.push_back(tempBoneInfo);
            }

            std::map<float, common::SQT> tempKeyFrames;

            for (unsigned int j = 0; j < channel->mNumPositionKeys; j++)
            {
                float time = channel->mPositionKeys[j].mTime;
                tempKeyFrames[time].position = glm::vec3(channel->mPositionKeys[j].mValue.x,
                                                         channel->mPositionKeys[j].mValue.y,
                                                         channel->mPositionKeys[j].mValue.z);
                tempKeyFrames[time].timeStamp = time;
                tempKeyFrames[time].jointName = channel->mNodeName.C_Str();
            }

            for (unsigned int j = 0; j < channel->mNumRotationKeys; j++)
            {
                float time = channel->mRotationKeys[j].mTime;
                tempKeyFrames[time].rotation = glm::quat(channel->mRotationKeys[j].mValue.w,
                                                         channel->mRotationKeys[j].mValue.x,
                                                         channel->mRotationKeys[j].mValue.y,
                                                         channel->mRotationKeys[j].mValue.z);
                tempKeyFrames[time].timeStamp = time;
                tempKeyFrames[time].jointName = channel->mNodeName.C_Str();
            }

            for (unsigned int j = 0; j < channel->mNumScalingKeys; j++)
            {
                float time = channel->mScalingKeys[j].mTime;
                tempKeyFrames[time].scale = glm::vec3(channel->mScalingKeys[j].mValue.x,
                                                      channel->mScalingKeys[j].mValue.y,
                                                      channel->mScalingKeys[j].mValue.z);
                tempKeyFrames[time].timeStamp = time;
                tempKeyFrames[time].jointName = channel->mNodeName.C_Str();
            }

            boneAnimation.keyFrames.reserve(tempKeyFrames.size());
            for (auto& [time, sqt] : tempKeyFrames)
                boneAnimation.keyFrames.push_back(sqt);

            boneAnimations.push_back(boneAnimation);
        }

        animation.boneAnimations = (boneAnimations);
        animations.push_back(animation);
    }

    //TODO Make texture and materials loading

    return Mesh{vertices, indices};
}

void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, std::unordered_map<std::string, unsigned int>& bonesMap, std::vector<common::BoneInfo> &bones, std::vector<common::Animation>&animations, glm::mat4& globalInverseTransform)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(processMesh(mesh, scene, bonesMap, bones, animations, globalInverseTransform));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene, meshes, bonesMap, bones, animations, globalInverseTransform);
}

Model AssetsManager::loadModel(const std::string &path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "AssetsManager::loadModels(): ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Could not load model " + path);
    }
    
    std::vector<Mesh> meshes;
    std::unordered_map<std::string, unsigned int> bonesMap;
    std::vector<common::BoneInfo> bones;
    std::vector<common::Animation> animations;
    glm::mat4 globalInverseTransform;

    processNode(scene->mRootNode, scene, meshes, bonesMap, bones, animations, globalInverseTransform);

    const std::filesystem::path file(path);

    const std::string name = file.filename().string();

    Model model{name, meshes};
    model.setBoneMap(bonesMap);
    model.setBonesInfo(bones);
    model.setAnimations(animations);
    model.globalInverseTransform = globalInverseTransform;
    model.rootNode = scene->mRootNode;

    for (const auto& bone : bones) {
        std::cout << "Bone: " << bone.name << " (ID: " << bone.id << "), Parent: "
                  << (bone.parentId == -1 ? "None" : bones[bone.parentId].name)
                  << ", Children: ";
        for (int child : bone.children) {
            std::cout << bones[child].name << " ";
        }
        std::cout << std::endl;
    }


    return model;
}

AssetsManager::~AssetsManager() = default;