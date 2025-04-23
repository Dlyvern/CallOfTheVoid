#include "Material.hpp"

#include <glad.h>
#include <glm/gtc/type_ptr.hpp>

Material::Material() = default;

Material::Material(const std::string &name) : m_name(name){}

std::string Material::getName() const
{
    return m_name;
}

void Material::setName(const std::string &name)
{
    m_name = name;
}

void Material::setBaseColor(const glm::vec3 &color)
{
    m_baseColor = color;
}

void Material::addTexture(const textures::TextureType &type, textures::Texture *texture)
{
    m_textures[type] = texture;

    if (type == textures::TextureType::Diffuse)
        m_useBaseColor = texture == nullptr;
}

textures::Texture* Material::getTexture(const textures::TextureType &type)
{
    return m_textures[type];
}

void Material::bind(Shader &shader)
{
    int textureUnit = 0;

    auto bindTex = [&shader, &textureUnit, this](textures::TextureType type, const std::string& uniformName, const std::string& enabledFlag)
    {
        auto texture = getTexture(type);

        shader.setInt(enabledFlag, texture != nullptr);

        if (texture)
        {
            texture->bind(textureUnit);
            shader.setInt(uniformName, textureUnit);
            textureUnit++;
        }
    };

    bindTex(textures::TextureType::Diffuse,   "u_Diffuse",   "use_Diffuse");
    bindTex(textures::TextureType::Normal,    "u_Normal",    "use_Normal");
    bindTex(textures::TextureType::Metallic,  "u_Metallic",  "use_Metallic");
    bindTex(textures::TextureType::Roughness, "u_Roughness", "use_Roughness");
    bindTex(textures::TextureType::AO,        "u_AO",        "use_AO");

    shader.setVec3("baseColor", m_baseColor);
}
