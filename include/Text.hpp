#ifndef TEXT_HPP
#define TEXT_HPP

#include "ft2build.h"
#include FT_FREETYPE_H

#include <glm/vec2.hpp>
#include <unordered_map>

#include "Shader.hpp"

//TODO: Always centralize text??
//TODO: Make 3d and 2d text
//TODO: Fix shitty default position
class Text
{
public:
    Text();


    //TODO: Maybe we can actually use font folder here, I think it will be stupid as fuck
    void setFont(const std::string& pathToFont);
    void setText(const std::string& text);
    void setX(float x);
    void setY(float y);
    void setPosition(float x, float y);
    void setScale(float scale);
    void setColor(const glm::vec3& color);


    float getX() const;
    float getY() const;

    void draw();

private:
    void initShader(const std::string& vertexPath, const std::string& fragmentPath);

    //TODO Do we actually need it here????
    struct Character
    {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };

    std::unordered_map<char, Character> m_characters;

    FT_Library m_ftLibrary;

    std::string m_fontPath;

    Shader m_shader;

    std::string m_text;

    glm::vec3 m_color{255.0f, 255.0f, 255.0f};

    float m_scale{1.0f};

    float m_x{500.0f};
    float m_y{500.0f};

    unsigned int m_vao;
    unsigned int m_vbo;
};

#endif //TEXT_HPP
