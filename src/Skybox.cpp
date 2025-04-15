#include "Skybox.hpp"

#include <glad.h>
#include <iostream>
#include <stb/stb_image.h>
#define STB_IMAGE_IMPLEMENTATION

#include <glm/ext/matrix_clip_space.hpp>

#include "CameraManager.hpp"
#include "Filesystem.hpp"
#include "WindowsManager.hpp"

unsigned int loadCubemap(const std::vector<std::string>& faces)
{
 unsigned int textureID;
 glGenTextures(1, &textureID);
 glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

 int width, height, nrComponents;

 for (unsigned int i = 0; i < faces.size(); i++)
 {
  unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
  if (data)
  {
   stbi_set_flip_vertically_on_load(faces[i] == filesystem::getTexturesFolderPath().string() + "/right.png");

   glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
   stbi_image_free(data);
  }
  else
  {
   std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
   stbi_image_free(data);
  }
 }
 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

 return textureID;
}


Skybox::Skybox() = default;

Skybox::Skybox(const std::vector<std::string> &faces)
{
    init(faces);
}

void Skybox::init(const std::vector<std::string> &faces)
{
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    auto folder = filesystem::getShadersFolderPath().string();
    m_skyboxShader.load(folder + "/skybox.vert", folder + "/skybox.frag");
    m_skyboxShader.bind();
    m_skyboxShader.setInt("skybox", 0);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    m_cubeMapTextureId = loadCubemap(faces);

}

void Skybox::render()
{
 glDepthFunc(GL_LEQUAL);
 const auto* window = window::WindowsManager::instance().getCurrentWindow();
 const auto* camera = CameraManager::getInstance().getActiveCamera();

 m_skyboxShader.bind();
 glm::mat4 skyboxProjection = glm::perspective(glm::radians(45.0f), static_cast<float>(window->getWidth()) / static_cast<float>(window->getHeight()), 0.1f, 100.0f);
 glm::mat4 skyboxView = glm::mat4(glm::mat3(camera->getViewMatrix()));
 m_skyboxShader.setMat4("view", skyboxView);
 m_skyboxShader.setMat4("projection", skyboxProjection);

 glBindVertexArray(m_vao);
 glActiveTexture(GL_TEXTURE0);
 glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMapTextureId);
 glDrawArrays(GL_TRIANGLES, 0, 36);
 glBindVertexArray(0);

 glDepthFunc(GL_LESS);

}
