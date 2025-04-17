#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "Texture.hpp"
#include "Shader.hpp"
#include <unordered_map>

class Material
{
public:
    void addTexture(const textures::TextureType& type, textures::Texture* texture);

    textures::Texture* getTexture(const textures::TextureType& type);

    void bind(Shader& shader);

private:
    std::unordered_map<textures::TextureType, textures::Texture*> m_textures;
    glm::vec3 m_baseColor = glm::vec3(128, 128, 128);
    bool m_useBaseColor = true;
};

#endif //MATERIAL_HPP
