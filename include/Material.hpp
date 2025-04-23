#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "Texture.hpp"
#include "Shader.hpp"
#include <unordered_map>

class Material
{
public:
    Material();

    explicit Material(const std::string& name);

    std::string getName() const;

    void setName(const std::string& name);

    void setBaseColor(const glm::vec3& color);

    void addTexture(const textures::TextureType& type, textures::Texture* texture);

    textures::Texture* getTexture(const textures::TextureType& type);

    void bind(Shader& shader);
private:
    std::string m_name{"Undefined"};
    std::unordered_map<textures::TextureType, textures::Texture*> m_textures;
    glm::vec3 m_baseColor = glm::vec3(128, 128, 128);
    bool m_useBaseColor = true;
};

#endif //MATERIAL_HPP
