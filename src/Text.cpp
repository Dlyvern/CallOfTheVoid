#include "Text.hpp"
#include <iostream>

#include "../libraries/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>

Text::Text(const std::string& vertexPath, const std::string& fragmentPath)
{
    if (FT_Init_FreeType(&m_ftLibrary))
    {
        //Are we stupid here?
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");
    }

    initShader(vertexPath, fragmentPath);
}

Text::Text()
{
    if (FT_Init_FreeType(&m_ftLibrary))
    {
        //Are we stupid here?
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");
    }
}

void Text::draw()
{
    m_shader.bind();


    float x = m_x;
    float y = m_y;

    m_shader.setVec3("textColor", m_color);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_vao);

    for (char c : m_text)
    {
        Character ch = m_characters[c];

        float xPosition = x + ch.Bearing.x * m_scale;
        float yPosition = y - (ch.Size.y - ch.Bearing.y) * m_scale;

        float w = ch.Size.x * m_scale;
        float h = ch.Size.y * m_scale;

        float vertices[6][4] =
        {
            { xPosition,     yPosition + h,   0.0f, 0.0f },
            { xPosition,     yPosition,       0.0f, 1.0f },
            { xPosition + w, yPosition,       1.0f, 1.0f },

            { xPosition,     yPosition + h,   0.0f, 0.0f },
            { xPosition + w, yPosition,       1.0f, 1.0f },
            { xPosition + w, yPosition + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * m_scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::initShader(const std::string &vertexPath, const std::string &fragmentPath)
{
    m_shader.load(vertexPath, fragmentPath);

    const glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1920), 0.0f, static_cast<float>(1080));
    m_shader.bind();
    m_shader.setMat4("projection", projection);
}

void Text::setFont(const std::string& pathToFont)
{
    if (pathToFont.empty())
    {
        std::cout << "Text::setFont(): Failed to load font, the given path is empty" << std::endl;
        return;
    }

    FT_Face face;

    if (FT_New_Face(m_ftLibrary, pathToFont.c_str(), 0, &face))
    {
        std::cout << "Text::setFont(): Failed to load font for some reason" << std::endl;
        return;
    }

    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };

        m_characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(m_ftLibrary);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Text::setText(const std::string &text)
{
    m_text = text;
}

void Text::setPosition(float x, float y)
{
    m_x = x;
    m_y = y;
}

void Text::setX(float x)
{
    m_x = x;
}

void Text::setY(float y)
{
    m_y = y;
}

void Text::setScale(float scale)
{
    m_scale = scale;
}

void Text::setColor(const glm::vec3 &color)
{
    m_color = color;
}
