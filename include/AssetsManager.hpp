#ifndef ASSETS_MANAGER_HPP
#define ASSETS_MANAGER_HPP

#include "Texture.hpp"

#include <unordered_map>
#include <iostream>
#include "SkinnedModel.hpp"
#include <set>

#include "Filesystem.hpp"
#include "Material.hpp"
#include "SkeletalMesh.hpp"
#include "StaticModel.hpp"

#include "Skeleton.hpp"

#include "StaticMesh.hpp"

class AssetsManager
{
public:
    static AssetsManager& instance();

    inline GLitch::Texture* getTextureByName(const std::string& name);
    inline SkinnedModel* getSkinnedModelByName(const std::string& name);
    inline StaticModel* getStaticModelByName(const std::string& name);
    inline common::Model* getModelByName(const std::string& name);
    std::vector<common::Model*> getAllLoadedModels() const;
    inline Material* getMaterialByName(const std::string& name);

    std::vector<std::string> getAllSkinnedModelsNames() const;
    std::vector<std::string> getAllStaticModelsNames() const;

    std::vector<SkinnedModel*> getAllSkinnedModels();
    std::vector<StaticModel*> getAllStaticModels();

    std::vector<Material*> getAllMaterials() ;

    Material* loadMaterialFromModel(aiMaterial* aiMat);

    void preLoadPathsForAllModels();
    void preLoadPathsForAllTextures();
    void preLoadPathsForAllMaterials();

    void preLoadModels(const std::vector<std::string>& paths);
    void preLoadTextures(const std::vector<std::string>& paths);
    void preLoadMaterials(const std::vector<std::string>& paths);

    //TODO Make private
    Material loadMaterial(const std::string& path);
    GLitch::Texture loadTexture(const std::string& path);
    SkinnedModel loadSkinnedModel(const std::string& path);
    StaticModel loadStaticModel(const std::string &path);

    std::vector<std::string> getAllLoadedModelsNames() const;
    std::vector<std::string> getAllLoadedMaterialsNames() const;

    std::vector<common::Animation> extractAnimationsFromModel(const std::string& pathToModel);

    ~AssetsManager();
private:
    std::unordered_map<std::string, GLitch::Texture> m_textures;
    std::unordered_map<std::string, SkinnedModel> m_skinnedModels;
    std::unordered_map<std::string, StaticModel> m_staticModels;
    std::unordered_map<std::string, Material> m_materials;

    std::set<std::string> m_staticModelsPaths;
    std::set<std::string> m_skinnedModelsPaths;
    std::set<std::string> m_materialsPaths;
    std::set<std::string> m_texturesPaths;
};

inline Material* AssetsManager::getMaterialByName(const std::string& name)
{
    if (m_materials.contains(name))
        return &m_materials[name];

    preLoadMaterials({name});

    if (m_materials.contains(name))
        return &m_materials[name];

    std::cout << "AssetsManager::getMaterialByName(): material '" << name << "' doesn't exist" << std::endl;

    return nullptr;
}

inline common::Model* AssetsManager::getModelByName(const std::string& name)
{
    if (m_staticModels.contains(name))
        return &m_staticModels[name];
    if (m_skinnedModels.contains(name))
        return &m_skinnedModels[name];

    std::cout << "AssetsManager::getModelByName(): '" << name << "' does not exist" << std::endl;

    return nullptr;
}

inline StaticModel* AssetsManager::getStaticModelByName(const std::string& name)
{
    if (m_staticModels.contains(name))
        return &m_staticModels[name];

    std::cout << "AssetsManager::getStaticModelByName(): '" << name << "' does not exist" << std::endl;

    return nullptr;
}

inline SkinnedModel* AssetsManager::getSkinnedModelByName(const std::string& name)
{
    if(auto it = m_skinnedModels.find(name); it != m_skinnedModels.cend())
        return &it->second;

    std::cout << "AssetsManager::getSkinnedModelByName(): '" << name << "' does not exist" << std::endl;
    
    return nullptr;
}

inline GLitch::Texture* AssetsManager::getTextureByName(const std::string& name)
{
    if(auto it = m_textures.find(name); it != m_textures.cend())
        return &it->second;

    auto folderPath = filesystem::getTexturesFolderPath().string() + "/" + name;

    if (std::filesystem::exists(folderPath))
    {
        preLoadTextures({name});

        if(auto it = m_textures.find(name); it != m_textures.cend())
            return &it->second;
    }


    std::cout << "AssetsManager::getTextureByName(): '" << name << "' does not exist" << std::endl;
    
    return nullptr;
}


#endif //ASSETS_MANAGER_HPP