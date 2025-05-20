#include "AssetsManager.hpp"

#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Common.hpp"

#include "Filesystem.hpp"

#include <unordered_map>
#include <map>
#include <json/json.hpp>

#include "Skeleton.hpp"
#include "Utilities.hpp"
#include <glm/gtx/euler_angles.hpp>
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

    common::Model::ModelType detectModelType(aiNode* node, const aiScene* scene, bool& foundSkinned)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
            if (scene->mMeshes[node->mMeshes[i]]->mNumBones > 0)
                foundSkinned = true;

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
            detectModelType(node->mChildren[i], scene, foundSkinned);

        return foundSkinned ? common::Model::ModelType::SKINNED : common::Model::ModelType::STATIC;
    };
} //namespace

AssetsManager& AssetsManager::instance()
{
    static AssetsManager assetsManager;
    return assetsManager;
}

GLitch::Texture AssetsManager::loadTexture(const std::string &path)
{
    return GLitch::Texture{path};
}

std::vector<common::Model*> AssetsManager::getAllLoadedModels() const
{
    std::vector<common::Model*> loadedModels;

    loadedModels.reserve(m_skinnedModels.size() + m_staticModels.size());

    for (auto m : m_skinnedModels)
        loadedModels.push_back(&m.second);

    for (auto m : m_staticModels)
        loadedModels.push_back(&m.second);

    return loadedModels;
}

std::vector<std::string> AssetsManager::getAllLoadedModelsNames() const
{
    std::vector<std::string> loadedModelNames;

    loadedModelNames.reserve(m_skinnedModels.size() + m_staticModels.size());

    for (const auto& m : m_skinnedModels)
        loadedModelNames.push_back(m.first);
    for (const auto& m : m_staticModels)
        loadedModelNames.push_back(m.first);

    return loadedModelNames;
}

std::vector<std::string> AssetsManager::getAllLoadedMaterialsNames() const
{
    std::vector<std::string> loadedMaterialNames;

    loadedMaterialNames.reserve(m_materials.size());

    for (const auto& material : m_materials)
        loadedMaterialNames.push_back(material.first);

    return loadedMaterialNames;
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

void AssetsManager::preLoadPathsForAllMaterials()
{
    auto materialsFolderPath = filesystem::getMaterialsFolderPath();

    if (!std::filesystem::exists(materialsFolderPath))
    {
        std::cerr << ("AssetsManager::preLoadPathsForAllMaterials(): materials folder does not exist") << std::endl;
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(materialsFolderPath))
    {
        if (entry.path().extension() == ".mat")
        {
            auto materialName = entry.path().string();

            if (m_materialsPaths.contains(materialName))
                continue;

            m_materialsPaths.insert(materialName);
        }
    }

    for (const auto& preLoadedMaterial : m_materialsPaths)
        std::cout << "Preloaded material: " << preLoadedMaterial << std::endl;
}

std::vector<std::string> AssetsManager::getAllSkinnedModelsNames() const
{
    std::vector<std::string> skinnedModels;

    skinnedModels.reserve(m_skinnedModels.size());

    for (const auto &key: m_skinnedModels | std::views::keys)
        skinnedModels.push_back(key);

    return skinnedModels;
}

std::vector<std::string> AssetsManager::getAllStaticModelsNames() const
{
    std::vector<std::string> staticModelNames;

    staticModelNames.reserve(m_staticModels.size());

    for (const auto& key: m_staticModels | std::views::keys)
        staticModelNames.push_back(key);

    return staticModelNames;
}

std::vector<SkinnedModel*> AssetsManager::getAllSkinnedModels()
{
    std::vector<SkinnedModel*> skinnedModels;

    skinnedModels.reserve(m_skinnedModels.size());

    for (auto& model : m_skinnedModels)
        skinnedModels.push_back(&model.second);

    return skinnedModels;
}

std::vector<StaticModel*> AssetsManager::getAllStaticModels()
{
    std::vector<StaticModel*> staticModels;

    staticModels.reserve(m_staticModels.size());

    for (auto& model : m_staticModels)
        staticModels.push_back(&model.second);

    return staticModels;
}

std::vector<Material*> AssetsManager::getAllMaterials()
{
    std::vector<Material*> materials;

    materials.reserve(m_materials.size());

    for (auto& material : m_materials)
        materials.push_back(&material.second);

    return materials;
}

Material* AssetsManager::loadMaterialFromModel(aiMaterial *aiMat)
{
    Material material;
    std::string modelDirectory;

    aiString name;

    if (aiMat->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
        material.setName(name.C_Str());

    if (m_materials.contains(material.getName()))
        return &m_materials[material.getName()];

    if (m_materialsPaths.contains(filesystem::getMaterialsFolderPath().string() + '/' + material.getName() + ".mat"))
    {
        preLoadMaterials({material.getName() + ".mat"});

        return getMaterialByName(material.getName() + ".mat");
    }

    if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;
        if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
        {
            std::string textureName = path.C_Str();
            std::ranges::replace(textureName, '\\', '/');

            size_t pos = textureName.find_last_of('/');

            if (pos != std::string::npos)
                textureName = textureName.substr(pos + 1, textureName.size() - pos);

            if (std::filesystem::path(textureName).extension() == ".jpg")
            {
                size_t extension = textureName.find_last_of('.');

                if (extension != std::string::npos)
                {
                    textureName = textureName.substr(0, extension);
                    textureName += ".png";
                }
            }

            auto* texture = getTextureByName(textureName);

            material.addTexture(GLitch::Texture::TextureType::Diffuse, texture);
        }
    }

    if (aiMat->GetTextureCount(aiTextureType_NORMALS) > 0)
    {
        aiString path;
        if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
        {
            std::string textureName = path.C_Str();
            std::ranges::replace(textureName, '\\', '/');

            size_t pos = textureName.find_last_of('/');

            if (pos != std::string::npos)
                textureName = textureName.substr(pos + 1, textureName.size() - pos);

            if (std::filesystem::path(textureName).extension() == ".jpg")
            {
                size_t extension = textureName.find_last_of('.');

                if (extension != std::string::npos)
                {
                    textureName = textureName.substr(0, extension);
                    textureName += ".png";
                }
            }

            std::cout << textureName << std::endl;

            auto* texture = getTextureByName(textureName);

            material.addTexture(GLitch::Texture::TextureType::Normal, texture);
        }
    }

    aiColor3D color(0.f, 0.f, 0.f);

    if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
    {
        material.setBaseColor(glm::vec3(color.r, color.g, color.b));
    }

    m_materials[material.getName()] = material;

    nlohmann::json json;

    json["name"] = material.getName() + ".mat";

    const auto& col = material.getBaseColor();

    json["color"] = {col.r, col.g, col.b};
    json["textures"] = {
        { "Diffuse", material.getTexture(GLitch::Texture::TextureType::Diffuse) ? material.getTexture(GLitch::Texture::TextureType::Diffuse)->getName() : "" },
        { "Normal", material.getTexture(GLitch::Texture::TextureType::Normal) ? material.getTexture(GLitch::Texture::TextureType::Normal)->getName() : "" },
        { "Metallic", material.getTexture(GLitch::Texture::TextureType::Metallic) ? material.getTexture(GLitch::Texture::TextureType::Metallic)->getName() : "" },
        { "Roughness", material.getTexture(GLitch::Texture::TextureType::Roughness) ? material.getTexture(GLitch::Texture::TextureType::Roughness)->getName() : "" },
        { "AO", material.getTexture(GLitch::Texture::TextureType::AO) ? material.getTexture(GLitch::Texture::TextureType::AO)->getName() : "" }
    };

    auto fileName = filesystem::getMaterialsFolderPath().string() + "/" + material.getName() + ".mat";

    std::ofstream outFile(fileName);

    if (outFile.is_open())
    {
        outFile << std::setw(4) << json << std::endl;
        outFile.close();
    }
    else
        std::cout << "Could not open file " << fileName << std::endl;

    return &m_materials[material.getName()];
}

void AssetsManager::saveAnimationToJson(const common::Animation &animation)
{
    nlohmann::json animationJson;

    animationJson["name"] = animation.name;
    animationJson["duration"] = animation.duration;
    animationJson["ticksPerSecond"] = animation.ticksPerSecond;

    for (const auto& boneAnimation : animation.boneAnimations)
    {
        nlohmann::json boneAnimationJson;

        boneAnimationJson["targetName"] = boneAnimation.objectName;

        for (const auto& sqt : boneAnimation.keyFrames)
        {
            nlohmann::json keyFramesJson;

            keyFramesJson["timeStamp"] = sqt.timeStamp;
            keyFramesJson["position"] = {sqt.position.x, sqt.position.y, sqt.position.z};
            keyFramesJson["rotation"] = {sqt.rotation.w, sqt.rotation.x, sqt.rotation.y, sqt.rotation.z};
            keyFramesJson["scale"] = {sqt.scale.x, sqt.scale.y, sqt.scale.z};

            boneAnimationJson["keyFrames"].push_back(keyFramesJson);
        }

        animationJson["tracks"].push_back(boneAnimationJson);
    }

    const std::string filePath = filesystem::getAnimationsFolderPath().string() + "/" + animation.name + ".anim";

    std::ofstream file(filePath);

    if (file.is_open())
    {
        file << std::setw(4) << animationJson << std::endl;
        file.close();
    }
    else
        std::cout << "Could not open file to save animation: " << filePath << std::endl;
}

common::Animation AssetsManager::loadAnimationFromJson(const std::string &path)
{
    std::ifstream file(path);
    nlohmann::json json;
    file >> json;

    common::Animation animation;

    animation.name = json["name"];
    animation.duration = json["duration"];
    animation.ticksPerSecond = json["ticksPerSecond"];

    for (const auto& boneAnimationJson : json["tracks"])
    {
        std::map<float, common::SQT> tempKeyFrames;

        common::AnimationTrack boneAnimation;
        boneAnimation.objectName = boneAnimationJson["targetName"];

        for (const auto& sqtJson : boneAnimationJson["keyFrames"])
        {
            common::SQT sqt;

            sqt.timeStamp = sqtJson["timeStamp"];
            const auto& position = sqtJson["position"];
            sqt.position = {position[0], position[1], position[2]};
            const auto& rotation = sqtJson["rotation"];

            glm::vec3 eulerDegrees = glm::vec3(rotation[0], rotation[1], rotation[2]);
            glm::vec3 eulerRadians = glm::radians(eulerDegrees);

            sqt.rotation = glm::quat_cast(glm::eulerAngleXYZ(eulerRadians.x, eulerRadians.y, eulerRadians.z));
            const auto& scale = sqtJson["scale"];
            sqt.scale = {scale[0], scale[1], scale[2]};

            tempKeyFrames[sqt.timeStamp] = sqt;

            boneAnimation.keyFrames.push_back(sqt);
        }

        // boneAnimation.keyFrames.push_back(sqt);

        // boneAnimation.keyFrames.reserve(tempKeyFrames.size());
        //
        // for (auto& [time, sqt] : tempKeyFrames)
        //     boneAnimation.keyFrames.push_back(sqt);

        animation.boneAnimations.push_back(boneAnimation);
    }

    return animation;
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

            modelType == common::Model::ModelType::STATIC ? m_staticModelsPaths.insert(entry.path().string()) : m_skinnedModelsPaths.insert(entry.path().string());
        }
    }
}

void AssetsManager::preLoadMaterials(const std::vector<std::string> &paths)
{
    for(const auto& path : paths)
    {
        if (m_materials.contains(path))
            continue;

        const auto materialsFolderPath = filesystem::getMaterialsFolderPath().string() + "/" + path;

        if (auto it = m_materialsPaths.find(path); it != m_materialsPaths.end())
        {
            m_materials[path] = loadMaterial(materialsFolderPath);
        }
        else
        {
            if (std::filesystem::exists(materialsFolderPath))
            {
                m_materialsPaths.insert(materialsFolderPath);

                m_materials[path] = loadMaterial(materialsFolderPath);
            }
            else
                std::cerr << "AssetsManager::preLoadMaterials(): Could not find " << path << std::endl;
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

                if (modelType == common::Model::ModelType::STATIC)
                {
                    m_staticModelsPaths.insert(modelsFolderPath);
                    m_staticModels[name] = loadStaticModel(modelsFolderPath);
                }
                else if(modelType == common::Model::ModelType::SKINNED)
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


void generateBoneHierarchy(const int parentId, const aiNode* src, Skeleton& skeleton, const glm::mat4& parentTransform)
{
    assert(src);

    std::string nodeName = src->mName.C_Str();
    glm::mat4 localTransform = utilities::convertMatrixToGLMFormat(src->mTransformation);
    glm::mat4 globalTransform = parentTransform * localTransform;

    // Clean up child name to avoid duplicates due to naming conventions
    size_t underscorePos = nodeName.find_first_of('_');
    
    if (underscorePos != std::string::npos)
        nodeName = nodeName.substr(0, underscorePos);

    int boneID = skeleton.getBoneId(nodeName);

    if (boneID == -1)
    {
        // If the bone isn't in the skeleton, create a new one to preserve hierarchy
        boneID = skeleton.getBonesCount();

        common::BoneInfo newBone(nodeName, boneID, utilities::convertMatrixToGLMFormat(src->mTransformation), glm::mat4(1.0f));

        boneID = skeleton.addBone(newBone);
    }

    common::BoneInfo* currentBone = skeleton.getBone(boneID);

    currentBone->globalBindTransform = globalTransform;

    if (auto parent = skeleton.getBone(parentId); parent && parent->id != boneID)
    {
        currentBone->parentId = parent->id;
        parent->children.push_back(boneID);
        parent->childrenInfo.push_back(currentBone);
    }

    for (unsigned int i = 0; i < src->mNumChildren; i++)
        generateBoneHierarchy(currentBone ? currentBone->id : -1, src->mChildren[i], skeleton, globalTransform);
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
        std::vector<common::AnimationTrack> boneAnimations;

        for(unsigned int animChannelIndex = 0; animChannelIndex < anim->mNumChannels; ++animChannelIndex)
        {
            aiNodeAnim* channel = anim->mChannels[animChannelIndex];
            common::AnimationTrack boneAnimation;

            boneAnimation.objectName = channel->mNodeName.C_Str();

            std::map<float, common::SQT> tempKeyFrames;

            for (unsigned int j = 0; j < channel->mNumPositionKeys; j++)
            {
                float time = channel->mPositionKeys[j].mTime;
                tempKeyFrames[time].position = glm::vec3(channel->mPositionKeys[j].mValue.x,
                                                         channel->mPositionKeys[j].mValue.y,
                                                         channel->mPositionKeys[j].mValue.z);
                tempKeyFrames[time].timeStamp = time;
            }

            for (unsigned int j = 0; j < channel->mNumRotationKeys; j++)
            {
                float time = channel->mRotationKeys[j].mTime;
                tempKeyFrames[time].rotation = glm::quat(channel->mRotationKeys[j].mValue.w,
                                                         channel->mRotationKeys[j].mValue.x,
                                                         channel->mRotationKeys[j].mValue.y,
                                                         channel->mRotationKeys[j].mValue.z);
                tempKeyFrames[time].timeStamp = time;
            }

            for (unsigned int j = 0; j < channel->mNumScalingKeys; j++)
            {
                float time = channel->mScalingKeys[j].mTime;
                tempKeyFrames[time].scale = glm::vec3(channel->mScalingKeys[j].mValue.x,
                                                      channel->mScalingKeys[j].mValue.y,
                                                      channel->mScalingKeys[j].mValue.z);
                tempKeyFrames[time].timeStamp = time;
            }

            boneAnimation.keyFrames.reserve(tempKeyFrames.size());

            for (auto& [time, sqt] : tempKeyFrames)
                boneAnimation.keyFrames.push_back(sqt);

            boneAnimations.push_back(boneAnimation);
        }

        animation.boneAnimations = (boneAnimations);
        animations.push_back(animation);
    }

    for (const auto& animation : animations)
        AssetsManager::instance().saveAnimationToJson(animation);

    return animations;
}

Material AssetsManager::loadMaterial(const std::string &path)
{
    std::ifstream file(path);
    nlohmann::json json;

    file >> json;

    Material material;

    if (json.contains("name"))
        material.setName(json["name"].get<std::string>());

    if (json.contains("color"))
    {
        auto color = json["color"];
        material.setBaseColor(glm::vec3(color[0].get<int>(), color[1].get<int>(), color[2].get<int>()));
    }

    if (json.contains("textures"))
        for (const auto& [key, value] : json["textures"].items())
        {
            if (!value.is_string() || value.get<std::string>().empty())
                continue;

            if (const auto textureType = utilities::fromStringToTextureType(key); textureType != GLitch::Texture::TextureType::Undefined)
                if (auto texture = getTextureByName(value.get<std::string>()))
                    material.addTexture(textureType, texture);
        }

    return material;
}

std::vector<common::Animation> AssetsManager::extractAnimationsFromModel(const std::string &pathToModel)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(pathToModel, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return {};
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

    skeleton.globalInverseTransform = glm::inverse(utilities::convertMatrixToGLMFormat(scene->mRootNode->mTransformation));
    generateBoneHierarchy(-1, scene->mRootNode, skeleton, glm::mat4(1.0f));
    assignLocalBindTransforms(scene->mRootNode, skeleton);
}

StaticMesh processStaticMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<common::Vertex> vertices;
    std::vector<unsigned int> indices;
    extractVerticesAndIndices(mesh, vertices, indices);

    StaticMesh staticMesh;

    staticMesh.setVerticesAndIndices(vertices, indices);

    unsigned int materialIndex = mesh->mMaterialIndex;

    aiMaterial* tmpMaterial = scene->mMaterials[materialIndex];

    if (tmpMaterial)
    {
        auto staticMaterial = AssetsManager::instance().loadMaterialFromModel(tmpMaterial);

        if (staticMaterial)
        {
            staticMesh.setMaterial(staticMaterial);
            std::cout << "Successfully loaded material " << staticMaterial->getName() << std::endl;
        }
        else
            std::cout << "Failed to load material" << std::endl;
    }
    else
        std::cout << "No material in the model" << std::endl;

    return staticMesh;
}

SkeletalMesh processSkeletalMesh(aiMesh* mesh, const aiScene* scene, Skeleton& skeleton)
{
    std::vector<common::Vertex> vertices;
    std::vector<unsigned int> indices;

    extractVerticesAndIndices(mesh, vertices, indices);
    extractSkeleton(mesh, scene, vertices, skeleton);

    SkeletalMesh skeletalMesh;
    skeletalMesh.setVerticesAndIndices(vertices, indices);

    unsigned int materialIndex = mesh->mMaterialIndex;

    aiMaterial* tmpMaterial = scene->mMaterials[materialIndex];

    if (tmpMaterial)
    {
        auto skeletonMaterial = AssetsManager::instance().loadMaterialFromModel(tmpMaterial);

        if (skeletonMaterial)
        {
            skeletalMesh.setMaterial(skeletonMaterial);
            std::cout << "Successfully loaded material " << skeletonMaterial->getName() << std::endl;
        }
        else
            std::cout << "Failed to load material" << std::endl;
    }
    else
        std::cout << "No material in the model" << std::endl;

    return skeletalMesh;
}

void processStaticModel(aiNode* node, const aiScene* scene, std::vector<StaticMesh>& meshes)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(processStaticMesh(mesh, scene));
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

    if (model.getName() == "mannequin.fbx")
    {
        auto animation = loadAnimationFromJson("/home/dlyvern/Projects/CallOfTheVoid/resources/animations/mixamo.com.anim");
        model.setAnimations({animation});
    }
    else
        model.setAnimations(extractAnimations(scene));

    model.setSkeleton(skeleton);

    return model;
}

AssetsManager::~AssetsManager() = default;