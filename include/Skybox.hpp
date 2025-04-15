#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include "Shader.hpp"
#include <vector>

class Skybox
{
public:
    Skybox();
    explicit Skybox(const std::vector<std::string>& faces);

    void init(const std::vector<std::string>& faces);

    void render();

private:
    Shader m_skyboxShader;
    unsigned int m_vao, m_vbo, m_ebo;
    unsigned int m_cubeMapTextureId;

};

#endif //SKYBOX_HPP
