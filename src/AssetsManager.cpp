#include "AssetsManager.hpp"

#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Common.hpp"

#include "Filesystem.hpp"
#include "Animation.hpp"

#include <unordered_map>

//TODO: FIX THAT SHIT
namespace
{
	static inline glm::mat4 convertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
        glm::mat4 to;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }

	static inline glm::vec3 getGLMVec(const aiVector3D& vec) 
	{ 
		return glm::vec3(vec.x, vec.y, vec.z); 
	}

	static inline glm::quat getGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}
}

AssetsManager& AssetsManager::instance()
{
    static AssetsManager assetsManager;
    return assetsManager;
}

void AssetsManager::initMinimim()
{
    auto texturesFolderPath = filesystem::getTexturesFolderPath();
    auto modelsFolderPath = filesystem::getModelsFolderPath();

    if(!std::filesystem::exists(texturesFolderPath))
        throw std::runtime_error("Texture folder does not exist");

    if(!std::filesystem::exists(modelsFolderPath))
        throw std::runtime_error("Models folder does not exist");

    textures::Texture textureVoid(std::filesystem::path(texturesFolderPath.string() + "/void.png").string());
    m_textures[textureVoid.getName()] = textureVoid;

    auto model = loadModel(modelsFolderPath.string() + "/void.fbx");
    m_models[model.getName()] = model;
}

void AssetsManager::loadTextures()
{
    auto texturesFolderPath = filesystem::getTexturesFolderPath();

    if(!std::filesystem::exists(texturesFolderPath))
        throw std::runtime_error("Texture folder does not exist");

    for(const auto& entry : std::filesystem::directory_iterator(texturesFolderPath))
    {
        if(entry.path().extension() == ".png")
        {
            textures::Texture texture{entry.path()};

            if(m_textures.find(texture.getName()) == m_textures.end())
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
        throw std::runtime_error("Models folder does not exist");

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

Mesh processMesh(aiMesh* mesh, const aiScene* scene, std::unordered_map<std::string, unsigned int>& bonesMap, std::vector<common::BoneInfo>& bones, std::vector<Animation>&animations)
{
    std::vector<common::Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<common::VertexBoneData> vertexBoneDataVector;

    for(unsigned int j = 0; j < mesh->mNumVertices; j++)
    {
        common::Vertex vertex;
        common::VertexBoneData vertexBoneData;

        for (int i = 0; i < 4; i++)
		{
			vertex.boneID[i] = -1;
			vertex.weight[i] = 0.0f;
		}

        vertex.position = {mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z};
        vertex.normal = {mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z};

        vertexBoneData.position = {mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z};
        vertexBoneData.normal = {mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z};

        if(mesh->HasTangentsAndBitangents())
        {
            vertex.tangent = {mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z};
            vertex.bitangent = {mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z};
            vertexBoneData.tangent = {mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z};
        }
        else
        {
            vertex.tangent = {0.0f, 0.0f, 0.0f};
            vertexBoneData.tangent = {0.0f, 0.0f, 0.0f};
            vertex.bitangent = {0.0f, 0.0f, 0.0f};
        }

        if(mesh->HasTextureCoords(0))
        {
            vertex.textureCoordinates = {mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y};
            vertexBoneData.uv =  {mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y};
        }
        else
        {
            vertexBoneData.uv =  {0.0f, 0.0f};
            vertex.textureCoordinates = {0.0f, 0.0f};
        }
        
        vertices.push_back(vertex);
        vertexBoneDataVector.push_back(vertexBoneData);
    }

    for(unsigned int k = 0; k < mesh->mNumFaces; k++) 
    {
        aiFace face = mesh->mFaces[k];

        for(unsigned int s = 0; s < face.mNumIndices; s++)
            indices.push_back(face.mIndices[s]);
    }

    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) 
    {
        aiBone* bone = mesh->mBones[boneIndex];
        std::string boneName = bone->mName.C_Str();

        int boneId{-1};

        if(bonesMap.find(boneName) == bonesMap.end())
        {
            common::BoneInfo boneInfo;

            boneInfo.name = boneName;
            boneInfo.offsetMatrix = convertMatrixToGLMFormat(bone->mOffsetMatrix);
            boneInfo.id = bones.size();
            boneInfo.finalTransformation = glm::mat4(1.0f);
            bonesMap[boneInfo.name] = boneInfo.id;

            bones.push_back(boneInfo);

            // std::cout << "Bone with name " << boneInfo.name << " and " << boneInfo.id << " index is loaded" << std::endl;
        }

        boneId = bonesMap[boneName];

        assert(boneId != -1);

        auto weights = bone->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (unsigned int bonesWeightIndex = 0; bonesWeightIndex < numWeights; bonesWeightIndex++)
        {
            // unsigned int boneIndexZ = bonesMap[boneName];
            unsigned int vertexIndex = weights[bonesWeightIndex].mVertexId;
            float weight = weights[bonesWeightIndex].mWeight;

            assert(vertexIndex <= vertices.size());

            common::Vertex& vertexBoneData = vertices[vertexIndex];

            for(int f = 0; f < 4; ++f)
            {
                if(vertexBoneData.boneID[f] < 0)
                {
                    vertexBoneData.weight[f] = weight;
                    vertexBoneData.boneID[f] = boneId;
                    break;
                }
            }

            // if (vertexBoneData.weight.x == 0) {
            //     vertexBoneData.boneID.x = boneIndexZ;
            //     vertexBoneData.weight.x = weight;
            // }
            // else if (vertexBoneData.weight.y == 0) {
            //     vertexBoneData.boneID.y = boneIndexZ;
            //     vertexBoneData.weight.y = weight;
            // }
            // else if (vertexBoneData.weight.z == 0) {
            //     vertexBoneData.boneID.z = boneIndexZ;
            //     vertexBoneData.weight.z = weight;
            // }
            // else if (vertexBoneData.weight.w == 0) {
            //     vertexBoneData.boneID.w = boneIndexZ;
            //     vertexBoneData.weight.w = weight;
            // }
        }
    }
    
    for(unsigned int i = 0; i < scene->mNumAnimations; ++i)
    {
        aiAnimation* anim = scene->mAnimations[i];
        Animation animation;
        animation.setName(anim->mName.C_Str());
        // std::cout << "Loaded animation with name " << anim->mName.C_Str() << std::endl;
        animation.setDuration(anim->mDuration);
        animation.setTicksPerSecond(anim->mTicksPerSecond);   
        std::vector<common::BoneAnimation> boneAnimations;

        for(unsigned int animChannelIndex = 0; animChannelIndex < anim->mNumChannels; ++animChannelIndex)
        {
            aiNodeAnim* channel = anim->mChannels[animChannelIndex];
            common::BoneAnimation boneAnimation;
            
            for (unsigned int j = 0; j < channel->mNumPositionKeys; j++) 
            {
                aiVector3D pos = channel->mPositionKeys[j].mValue;
                common::SQT sqt;
                sqt.position = {pos.x, pos.y, pos.z};
                sqt.timeStamp = channel->mPositionKeys[j].mTime;
                sqt.jointName = channel->mNodeName.C_Str();
                boneAnimation.keyFrames.push_back(sqt);
            }

            for (unsigned int j = 0; j < channel->mNumRotationKeys; j++) 
            {
                aiQuaternion rot = channel->mRotationKeys[j].mValue;
                common::SQT sqt;
                sqt.rotation = {rot.w, rot.x, rot.y, rot.z};
                sqt.timeStamp = channel->mRotationKeys[j].mTime;
                sqt.jointName = channel->mNodeName.C_Str();
                boneAnimation.keyFrames.push_back(sqt);
            }

            for (unsigned int j = 0; j < channel->mNumScalingKeys; j++)
            {
                aiVector3D scale = channel->mScalingKeys[j].mValue;
                common::SQT sqt;
                sqt.scale = scale.x;
                sqt.timeStamp = channel->mScalingKeys[j].mTime;
                sqt.jointName = channel->mNodeName.C_Str();
                boneAnimation.keyFrames.push_back(sqt);
            }

            boneAnimations.push_back(boneAnimation);
        }

        animation.setBoneAnimations(boneAnimations);
        animations.push_back(animation);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    
    //TODO Make texture loading
    // if(scene->mNumMaterials)
    // {
    //     aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    //     for(int i = 0; i < scene->mNumMaterials; i++)
    //     {
    //         aiMaterial* material = scene->mMaterials[i];
    //         std::cout << "Material №" << i <<  " textures count: " << material->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;
        
    //     }

    // }

    // for(int i = 0; i < scene->mNumMaterials; i++)
    // {
    //     auto material = scene->mMaterials[i];

    //     std::cout << material->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;

    //     for(int j = 0; j < material->GetTextureCount(aiTextureType_DIFFUSE); j++)
    //     {
    //         aiString path;

    //         if (material->GetTexture(aiTextureType_DIFFUSE, j, &path) == AI_SUCCESS)
    //         {
    //             std::cout << "FOUND SOMETHING " << path.C_Str() << std::endl;
    //         }
    //     }
    // }


    return Mesh{vertices, indices, vertexBoneDataVector};
}

void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, std::unordered_map<std::string, unsigned int>& bonesMap, std::vector<common::BoneInfo> &bones, std::vector<Animation>&animations)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(processMesh(mesh, scene, bonesMap, bones, animations));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene, meshes, bonesMap, bones, animations);
}

Model AssetsManager::loadModel(const std::string &path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "AssetsManager::loadModels(): ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Could not load model");
    }
    
    std::vector<Mesh> meshes;
    std::unordered_map<std::string, unsigned int> bonesMap;
    std::vector<common::BoneInfo> bones;
    std::vector<Animation> animations;

    processNode(scene->mRootNode, scene, meshes, bonesMap, bones, animations);

    std::filesystem::path file(path);
    
    std::string name = file.filename().string();

    Model model{name, meshes};
    model.setBoneMap(bonesMap);
    model.setBonesInfo(bones);
    model.setAnimations(animations);

    return model;
}

AssetsManager::~AssetsManager() = default;