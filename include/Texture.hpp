#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>

namespace textures
{
    struct TextureData
    {
    public:
        int width;
        int height;
        int numberOfChannels;
        unsigned char* data;
    };

    enum class TextureType
    {
        Diffuse,
        Specular,
        Normal,
        Metallic,
        Roughness,
        AO,
        Emissive,
        Height,
        Glossiness,
        Opacity,
        Undefined
    };

    inline TextureType fromStringToTextureType(const std::string& type)
    {
        if (type == "Diffuse")
            return TextureType::Diffuse;
        if (type == "Specular")
            return TextureType::Specular;
        if (type == "Normal")
            return TextureType::Normal;
        if (type == "Metallic")
            return TextureType::Metallic;
        if (type == "Roughness")
            return TextureType::Roughness;
        if (type == "AO")
            return TextureType::AO;
        if (type == "Emissive")
            return TextureType::Emissive;
        if (type == "Height")
            return TextureType::Height;
        if (type == "Glossiness")
            return TextureType::Glossiness;
        if (type == "Opacity")
            return TextureType::Opacity;
        else
            return TextureType::Undefined;
    }

    class Texture
    {
    public:
        explicit Texture(const std::string& filePath);

        Texture();

        void load(const std::string& filePath);

        [[nodiscard]]const std::string& getName() const;

        [[nodiscard]]unsigned int getId() const;

        void bake();

        bool isBaked() const;

        void bind(unsigned int slot = 0);

        ~Texture();

    private:
        bool m_isBaked;
        unsigned int m_id;
        TextureData m_textureData;
        std::string m_name;
    };

} //Textures

#endif //TEXTURE_HPP