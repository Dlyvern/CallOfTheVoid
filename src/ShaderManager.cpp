#include "ShaderManager.hpp"
#include "Filesystem.hpp"
#include "Skeleton.hpp"

ShaderManager& ShaderManager::instance()
{
    static ShaderManager instance;
    return instance;
}

GLitch::Shader* ShaderManager::getShader(const ShaderType& type)
{
    return &m_shaders[type];
}

void ShaderManager::preLoadShaders()
{
    const std::string shadersPath = filesystem::getShadersFolderPath().string();

    auto createShader = [](const std::string& vert, const std::string& frag)
    {
        GLitch::Shader shader;
        shader.load(vert, frag);
        return shader;
    };

    m_shaders[SKELETON] = createShader(shadersPath + "/skeleton.vert", shadersPath + "/skeleton.frag");
    m_shaders[STATIC] = createShader(shadersPath + "/cube.vert", shadersPath + "/cube.frag");
    m_shaders[STATIC_SHADOW] = createShader(shadersPath + "/shadow_map.vert", shadersPath + "/shadow_map.frag");
    m_shaders[SKELETON_SHADOW] = createShader(shadersPath + "/shadow.vert", shadersPath + "/shadow.frag");
    m_shaders[POST_PROCESSING] = createShader(shadersPath + "/post_processing.vert", shadersPath + "/post_processing.frag");
}
