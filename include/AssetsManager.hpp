#ifndef ASSETS_MANAGER_HPP
#define ASSETS_MANAGER_HPP

#include "Texture.hpp"

#include <unordered_map>
#include <iostream>
#include "Model.hpp"
#include <set>

class AssetsManager
{
public:
    static AssetsManager& instance();

    inline textures::Texture* getTextureByName(const std::string& name);

    inline Model* getModelByName(const std::string& name);

    void initMinimum();

    void loadTextures();

    void loadModels();

    Model loadModel(const std::string& path);

    ~AssetsManager();
    std::unordered_map<std::string, Model> m_models;

private:
    std::unordered_map<std::string, textures::Texture> m_textures;

    std::set<std::string> m_modelsPaths;
};

inline Model* AssetsManager::getModelByName(const std::string& name)
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