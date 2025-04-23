#include "Texture.hpp"

#include "glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../libraries/stb/stb_image.h"

#include <iostream>

#include <filesystem>

//Maybe fix it later...
namespace
{
    textures::TextureData getTextureDataFromFile(const std::string& filePath, const bool stbiSetFlipVerticallyOnLoad = false)
    {
        stbi_set_flip_vertically_on_load(stbiSetFlipVerticallyOnLoad);

        textures::TextureData textureData{};

        textureData.data = stbi_load(filePath.data(), &textureData.width, &textureData.height, &textureData.numberOfChannels, 0);

        if(!textureData.data)
            std::cout << "Failed to load a texture";

        return textureData;
    }
}

textures::Texture::Texture() = default;

textures::Texture::Texture(const std::string &filePath)
{
    load(filePath);
}

void textures::Texture::load(const std::string &filePath)
{
    m_textureData = getTextureDataFromFile(filePath);

    std::filesystem::path file(filePath);
    m_name = file.filename().string();
}

const std::string &textures::Texture::getName() const
{
    return m_name;
}

void textures::Texture::bake()
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    GLint format = GL_RGB;

    if (m_textureData.numberOfChannels == 4)
        format = GL_RGBA;
    else if (m_textureData.numberOfChannels == 3)
        format = GL_RGB;
    else if (m_textureData.numberOfChannels == 1)
        format = GL_RED;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_textureData.width, m_textureData.height, 0, format, GL_UNSIGNED_BYTE, m_textureData.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(m_textureData.data);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_textureData.data = nullptr;
    m_isBaked = true;
}

bool textures::Texture::isBaked() const
{
    return m_isBaked;
}

void textures::Texture::bind(unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

textures::Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

GLuint textures::Texture::getId() const
{
    return m_id;
}
