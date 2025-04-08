#ifndef ASSETS_MANAGER_HPP
#define ASSETS_MANAGER_HPP

#include "Texture.hpp"

#include <unordered_map>
#include <iostream>
#include "SkinnedModel.hpp"
#include <set>

#include "SkeletalMesh.hpp"
#include "StaticModel.hpp"

#include "Skeleton.hpp"

#include "StaticMesh.hpp"

class AssetsManager
{
public:
    static AssetsManager& instance();

    inline textures::Texture* getTextureByName(const std::string& name);

    inline SkinnedModel* getSkinnedModelByName(const std::string& name);

    inline StaticModel* getStaticModelByName(const std::string& name);

    inline common::Model* getModelByName(const std::string& name);

    void loadTextures();

    void loadModels();

    void preLoadPathsForAllModels();

    void preLoadPathsForAllTextures();

    void preLoadModels(const std::vector<std::string>& paths);

    void preLoadTextures(const std::vector<std::string>& paths);


    //TODO Make private
    SkinnedModel loadSkinnedModel(const std::string& path);
    StaticModel loadStaticModel(const std::string &path);

    textures::Texture loadTexture(const std::string& path);

    ~AssetsManager();
private:
    std::unordered_map<std::string, textures::Texture> m_textures;
    std::unordered_map<std::string, SkinnedModel> m_skinnedModels;
    std::unordered_map<std::string, StaticModel> m_staticModels;

    std::set<std::string> m_staticModelsPaths;
    std::set<std::string> m_skinnedModelsPaths;

    std::set<std::string> m_texturesPaths;
};

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

inline textures::Texture* AssetsManager::getTextureByName(const std::string& name)
{
    if(auto it = m_textures.find(name); it != m_textures.cend())
        return &it->second;

    std::cout << "AssetsManager::getTextureByName(): '" << name << "' does not exist" << std::endl;
    
    return nullptr;
}


#endif //ASSETS_MANAGER_HPP