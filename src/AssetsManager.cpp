#include "AssetsManager.hpp"

#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Common.hpp"

#include "Filesystem.hpp"

#include <unordered_map>
#include <map>

#include "Skeleton.hpp"
#include "Utilities.hpp"

namespace
{
    void extractVerticesAndIndices(aiMesh* mesh, std::vector<common::Vertex>& vertices, std::vector<unsigned int>& indices)
    {
        for(unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            common::Vertex vertex;

            for (int i = 0; i < 4; i++)
            {
                vertex.boneID[i] = -1;
                vertex.weight[i] = 0.0f;
            }

            common::AABB boundingBox{};

            boundingBox.min.x = std::min(boundingBox.min.x, mesh->mVertices[j].x);
            boundingBox.min.y = std::min(boundingBox.min.y, mesh->mVertices[j].y);
            boundingBox.min.z = std::min(boundingBox.min.z, mesh->mVertices[j].z);

            boundingBox.max.x = std::max(boundingBox.max.x, mesh->mVertices[j].x);
            boundingBox.max.y = std::max(boundingBox.max.y, mesh->mVertices[j].y);
            boundingBox.max.z = std::max(boundingBox.max.z, mesh->mVertices[j].z);

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
    }

    enum class ModelType : uint8_t
    {
        SKINNED = 0,
        STATIC = 1
    };

    ModelType detectModelType(aiNode* node, const aiScene* scene, bool& foundSkinned)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
            if (scene->mMeshes[node->mMeshes[i]]->mNumBones > 0)
                foundSkinned = true;

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
            detectModelType(node->mChildren[i], scene, foundSkinned);

        return foundSkinned ? ModelType::SKINNED : ModelType::STATIC;
    };
} //namespace

AssetsManager& AssetsManager::instance()
{
    static AssetsManager assetsManager;
    return assetsManager;
}

textures::Texture AssetsManager::loadTexture(const std::string &path)
{
    return textures::Texture{path};
}

void AssetsManager::preLoadPathsForAllTextures()
{
    const std::filesystem::path texturesFolderPath = filesystem::getTexturesFolderPath();

    if (!std::filesystem::exists(texturesFolderPath))
    {
        std::cerr << "AssetsManager::preLoadPathsForAllTextures(): Texture folder does not exist: " << texturesFolderPath.string() << std::endl;
        return;
    }

    for(const auto& entry : std::filesystem::directory_iterator(texturesFolderPath))
    {
        if (entry.path().extension() == ".png")
        {
            auto textureName = entry.path().string();

            if (m_texturesPaths.contains(textureName))
                continue;

            m_texturesPaths.insert(textureName);
        }
    }
}

void AssetsManager::preLoadPathsForAllModels()
{
    auto modelsFolderPath = filesystem::getModelsFolderPath();

    if(!std::filesystem::exists(modelsFolderPath))
    {
        std::cerr << ("AssetsManager::preLoadPathsForAllModels(): models folder does not exist") << std::endl;
        return;
    }

    for(const auto& entry : std::filesystem::directory_iterator(modelsFolderPath))
    {
        if(entry.path().extension() == ".obj" || entry.path().extension() == ".fbx")
        {
            std::filesystem::path file(entry.path());

            Assimp::Importer importer;

            const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);

            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                std::cerr << "AssetsManager::preLoadPathsForAllModels(): ERROR::ASSIMP::" << importer.GetErrorString();
                continue;
            }

            bool foundSkinned = false;
            auto modelType = detectModelType(scene->mRootNode, scene, foundSkinned);

            modelType == ModelType::STATIC ? m_staticModelsPaths.insert(entry.path().string()) : m_skinnedModelsPaths.insert(entry.path().string());
        }
    }
}

void AssetsManager::preLoadModels(const std::vector<std::string> &paths)
{
    for (const auto& name : paths)
    {
        if (m_skinnedModels.contains(name) || m_staticModels.contains(name))
            continue;

        const auto modelsFolderPath = std::string(filesystem::getModelsFolderPath()) + "/" + name;

        if (auto it =  m_skinnedModelsPaths.find(modelsFolderPath); it != m_skinnedModelsPaths.end())
            m_skinnedModels[name] = loadSkinnedModel(modelsFolderPath);
        else if (auto it = m_staticModelsPaths.find(modelsFolderPath); it != m_staticModelsPaths.end())
            m_staticModels[name] = loadStaticModel(modelsFolderPath);
        else
        {
            //Global checking
            if (std::filesystem::exists(modelsFolderPath))
            {
                std::filesystem::path file(modelsFolderPath);

                Assimp::Importer importer;

                const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);

                if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
                {
                    std::cerr << "AssetsManager::preLoadModels(): ERROR::ASSIMP::" << importer.GetErrorString();
                    continue;
                }

                bool foundSkinned = false;
                const auto modelType = detectModelType(scene->mRootNode, scene, foundSkinned);

                if (modelType == ModelType::STATIC)
                {
                    m_staticModelsPaths.insert(modelsFolderPath);
                    m_staticModels[name] = loadStaticModel(modelsFolderPath);
                }
                else if(modelType == ModelType::SKINNED)
                {
                    m_skinnedModelsPaths.insert(modelsFolderPath);
                    m_skinnedModels[name] = loadSkinnedModel(modelsFolderPath);
                }
            }
            else
                std::cerr << "AssetsManager::preLoadModels(): could not find the given model " << name << "\n";
        }
    }
}

void AssetsManager::preLoadTextures(const std::vector<std::string> &paths)
{
    for (const auto& name : paths)
    {
        if (m_textures.contains(name))
            continue;

        auto texturesFolderPath = std::string(filesystem::getTexturesFolderPath()) + "/" + name;

        if (auto it = m_texturesPaths.find(texturesFolderPath); it != m_texturesPaths.end())
            m_textures[name] = loadTexture(*it);
    }
}


void generateBoneHierarchy(const int parentId, const aiNode* src, Skeleton& skeleton)
{
    assert(src);

    std::string nodeName = src->mName.C_Str();
    // Clean up child name to avoid duplicates due to naming conventions
    size_t underscorePos = nodeName.find_first_of('_');
    
    if (underscorePos != std::string::npos)
        nodeName = nodeName.substr(0, underscorePos);

    int boneID = skeleton.getBoneId(nodeName);

    if (boneID == -1)
    {
        // If the bone isn't in the skeleton, create a new one to preserve hierarchy
        boneID = skeleton.getBones().size();

        common::BoneInfo newBone(nodeName, boneID, utilities::convertMatrixToGLMFormat(src->mTransformation), glm::mat4(1.0f));

        boneID = skeleton.addBone(newBone);
    }

    common::BoneInfo* currentBone = skeleton.getBone(boneID);

    if (auto parent = skeleton.getBone(parentId); parent && parent->id != boneID)
    {
        currentBone->parentId = parent->id;
        parent->children.push_back(boneID);
        parent->childrenInfo.push_back(currentBone);
    }

    for (unsigned int i = 0; i < src->mNumChildren; i++)
        generateBoneHierarchy(currentBone ? currentBone->id : -1, src->mChildren[i], skeleton);
}

std::vector<common::Animation> extractAnimations(const aiScene* scene)
{
    std::vector<common::Animation> animations;

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
            // bool cool = false;
            // for (const auto& b : bones)
            //     if (b.name == boneAnimation.boneName)
            //         cool = true;
            //
            // if (!cool) {
            //     common::BoneInfo tempBoneInfo;
            //     tempBoneInfo.name = boneAnimation.boneName;
            //     bones.push_back(tempBoneInfo);
            // }

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

    return animations;
}

std::vector<common::Animation> AssetsManager::extractAnimationsFromModel(const std::string &pathToModel)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(pathToModel, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "AssetsManager::extractAnimationsFromModel(): ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Could not load model " + pathToModel);
    }

    return extractAnimations(scene);
}


void assignLocalBindTransforms(aiNode* node, Skeleton& skeleton)
{
    if (auto bone = skeleton.getBone(node->mName.C_Str()))
        bone->localBindTransform = utilities::convertMatrixToGLMFormat(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        assignLocalBindTransforms(node->mChildren[i], skeleton);
}

void extractSkeleton(const aiMesh* mesh, const aiScene* scene, std::vector<common::Vertex>& vertices, Skeleton& skeleton)
{
    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
    {
        aiBone* bone = mesh->mBones[boneIndex];

        skeleton.addBone(bone);

        std::string boneName = bone->mName.C_Str();
        const unsigned int boneId = skeleton.getBoneId(boneName);

        for (unsigned int bonesWeightIndex = 0; bonesWeightIndex < bone->mNumWeights; bonesWeightIndex++)
        {
            const auto boneWeight = bone->mWeights[bonesWeightIndex];

            common::Vertex& vertexBoneData = vertices[boneWeight.mVertexId];

            for(int f = 0; f < 4; ++f)
            {
                if(vertexBoneData.boneID[f] < 0)
                {
                    vertexBoneData.weight[f] = boneWeight.mWeight;
                    vertexBoneData.boneID[f] = boneId;
                    break;
                }
            }
        }
    }

    generateBoneHierarchy(-1, scene->mRootNode, skeleton);
    assignLocalBindTransforms(scene->mRootNode, skeleton);

    for (auto& boneData : skeleton.getBones())
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

                if (skeleton.getBoneMap().contains(childName) && childName != boneNode->mName.C_Str())
                {
                    int childID = skeleton.getBoneMap()[childName];

                    if (std::ranges::find_if(skeleton.getBones()[boneData.id].children, [&childID](int child) {  return child == childID; }) == skeleton.getBones()[boneData.id].children.end())
                    {
                        skeleton.getBones()[boneData.id].children.push_back(childID);
                        skeleton.getBones()[boneData.id].childrenInfo.push_back(&skeleton.getBones()[childID]);
                    }

                }
            }
        }
    }

    // //TODO THIS CODE IS VERYYYYYYYYYYYYYYY SLOW
    // for (auto& parentBone : skeleton.getBones())
    //     for (int childID : parentBone.children)
    //         if (skeleton.getBones()[childID].parentId == -1)
    //             skeleton.getBones()[childID].parentId = parentBone.id;
}

StaticMesh processStaticMesh(aiMesh* mesh)
{
    std::vector<common::Vertex> vertices;
    std::vector<unsigned int> indices;

    extractVerticesAndIndices(mesh, vertices, indices);
    // glm::mat4 globalInverseTransform = utilities::convertMatrixToGLMFormat(scene->mRootNode->mTransformation);

    return {vertices, indices};
}

SkeletalMesh processSkeletalMesh(aiMesh* mesh, const aiScene* scene, Skeleton& skeleton)
{
    std::vector<common::Vertex> vertices;
    std::vector<unsigned int> indices;

    extractVerticesAndIndices(mesh, vertices, indices);
    extractSkeleton(mesh, scene, vertices, skeleton);

    return {vertices, indices};
}

void processStaticModel(aiNode* node, const aiScene* scene, std::vector<StaticMesh>& meshes)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(processStaticMesh(mesh));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        processStaticModel(node->mChildren[i], scene, meshes);
}

void processSkinnedModel(aiNode* node, const aiScene* scene, std::vector<SkeletalMesh>& skeletalMeshes, Skeleton& skeleton)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        skeletalMeshes.emplace_back(processSkeletalMesh(mesh, scene, skeleton));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        processSkinnedModel(node->mChildren[i], scene, skeletalMeshes, skeleton);
}

StaticModel AssetsManager::loadStaticModel(const std::string &path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "AssetsManager::loadStaticModel(): ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Could not load model " + path);
    }

    std::vector<StaticMesh> staticMeshes;
    const std::string name = std::filesystem::path(path).filename().string();

    processStaticModel(scene->mRootNode, scene, staticMeshes);

    return {name, staticMeshes};
}

SkinnedModel AssetsManager::loadSkinnedModel(const std::string &path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "AssetsManager::loadModel(): ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Could not load model " + path);
    }

    Skeleton skeleton;
    std::vector<SkeletalMesh> meshes;
    processSkinnedModel(scene->mRootNode, scene, meshes, skeleton);

    const std::string name = std::filesystem::path(path).filename().string();

    SkinnedModel model{name, meshes};

    model.setAnimations(extractAnimations(scene));
    model.setSkeleton(skeleton);

    return model;
}

AssetsManager::~AssetsManager() = default;