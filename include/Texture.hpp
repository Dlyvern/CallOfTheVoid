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
        Opacity
    };

    class Texture
    {
    public:
        explicit Texture(const std::string& filePath);

        Texture();

        void load(const std::string& filePath);

        [[nodiscard]]const std::string& getName() const;

        [[nodiscard]]unsigned int getId() const;

        void bake();

        void bind(unsigned int slot = 0);

        ~Texture();

    private:
        unsigned int m_id;
        TextureData m_textureData;
        std::string m_name;
    };

} //Textures

#endif //TEXTURE_HPP