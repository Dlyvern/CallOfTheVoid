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

    inline SkinnedModel* getModelByName(const std::string& name);

    void initMinimum();

    void loadTextures();

    void loadModels();

    SkinnedModel loadSkinnedModel(const std::string& path);

    StaticModel loadStaticModel(const std::string &path);

    ~AssetsManager();
    std::unordered_map<std::string, SkinnedModel> m_models;

private:
    std::unordered_map<std::string, textures::Texture> m_textures;

    std::set<std::string> m_modelsPaths;
};

inline SkinnedModel* AssetsManager::getModelByName(const std::string& name)
{
    if(auto it = m_models.find(name); it != m_models.cend())
        return &it->second;

    std::cout << "AssetsManager::getModelByName(): '" << name << "' does not exist" << std::endl;
    
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