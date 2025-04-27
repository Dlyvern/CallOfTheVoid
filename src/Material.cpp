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

void Material::addTexture(const GLitch::Texture::TextureType &type, GLitch::Texture *texture)
{
    m_textures[type] = texture;

    if (type == GLitch::Texture::TextureType::Diffuse)
        m_useBaseColor = texture == nullptr;
}

GLitch::Texture* Material::getTexture(const GLitch::Texture::TextureType &type)
{
    return m_textures[type];
}

const glm::vec3& Material::getBaseColor() const
{
    return m_baseColor;
}

void Material::bind(GLitch::Shader &shader)
{
    int textureUnit = 0;

    auto bindTex = [&shader, &textureUnit, this](GLitch::Texture::TextureType type, const std::string& uniformName, const std::string& enabledFlag)
    {
        auto texture = getTexture(type);

        shader.setInt(enabledFlag, texture != nullptr);

        if (texture)
        {
            if (!texture->isBaked())
                texture->bake();

            texture->bind(textureUnit);
            shader.setInt(uniformName, textureUnit);
            textureUnit++;
        }
    };

    bindTex(GLitch::Texture::TextureType::Diffuse,   "u_Diffuse",   "use_Diffuse");
    bindTex(GLitch::Texture::TextureType::Normal,    "u_Normal",    "use_Normal");
    bindTex(GLitch::Texture::TextureType::Metallic,  "u_Metallic",  "use_Metallic");
    bindTex(GLitch::Texture::TextureType::Roughness, "u_Roughness", "use_Roughness");
    bindTex(GLitch::Texture::TextureType::AO,        "u_AO",        "use_AO");

    shader.setVec3("baseColor", m_baseColor);
}
