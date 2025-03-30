#include "LoadingScene.hpp"
#include "Shader.hpp"
#include "Filesystem.hpp"
#include "../libraries/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Texture.hpp"
#include "AssetsManager.hpp"
#include "Cube.hpp"
#include <map>
#include <future>

#include "CameraManager.hpp"
#include "Void.hpp"

#include "ft2build.h"
#include FT_FREETYPE_H

namespace
{
    struct Character 
    {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };

    std::map<char, Character> Characters;
    textures::Texture* texture;
    geometries::Cube cube{"void"};

    Void voidObject("Void");

    Shader shader;
    unsigned int VAO, VBO;
}

LoadingScene::LoadingScene() : Scene()
{
}

void LoadingScene::create()
{
    texture = AssetsManager::instance().getTextureByName("void.png");
    texture->bake();

    voidObject.create();

    voidObject.setTexture(texture);
    voidObject.setModel(AssetsManager::instance().getModelByName("void.fbx"));
    voidObject.setScale(glm::vec3{0.01f, 0.01f, 0.01f});
    voidObject.setRotation(180, glm::vec3{0.0, 1.0, 0.0});
    voidObject.setPosition({5.0f, -5.0f, -20.0f});

    auto shadersDirectory = filesystem::getShadersFolderPath();
    shader.load(shadersDirectory.string() + "/text.vert", shadersDirectory.string() + "/text.frag");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1920), 0.0f, static_cast<float>(1080));
    shader.bind();
    shader.setMat4("projection", projection);

    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        // return -1;
    }

	// find path to font
    std::string font_name = filesystem::getFontsFolderPath().string() + "/ghostmane_font.ttf";
    
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        // return -1;
    }
	
	// load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        // return -1;
    }
    else {
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
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // m_future = std::async(std::launch::async, [this]{loadAllAssets();});
}

void LoadingScene::loadAllAssets()
{
    auto texturesFuture = std::async(std::launch::async, []{AssetsManager::instance().loadTextures();});
    auto modelsFuture = std::async(std::launch::async, []{AssetsManager::instance().loadModels();});
    // AssetsManager::instance().loadTextures();
    // AssetsManager::instance().loadModels();

    while(true)
        if(texturesFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready && modelsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            break;
}


void LoadingScene::update(float deltaTime)
{
    CameraManager::getInstance().getActiveCamera()->update(deltaTime);

    shader.bind();   

    std::string text{"CALL OF THE VOID"};
    float x{500.0f};
    float y{500.0f};
    float scale{1.0f};
    glm::vec3 color{255.0f, 255.0f, 255.0f};

    glUniform3f(glGetUniformLocation(shader.getId(), "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);


    voidObject.update(deltaTime);
    // voidObject.rotate(true);

    // if(m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    //     m_endScene = true;
}

bool LoadingScene::isOver()
{
    // return m_endScene;
    return false;
}

LoadingScene::~LoadingScene() = default;

