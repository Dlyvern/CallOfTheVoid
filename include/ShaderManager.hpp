#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <unordered_map>
#include "Shader.hpp"

class ShaderManager
{
public:
    enum ShaderType : uint16_t
    {
        SKELETON = 0,
        STATIC = 1,
        STATIC_SHADOW = 2,
        SKELETON_SHADOW = 3,
    };

    static ShaderManager& instance();

    GLitch::Shader* getShader(const ShaderType& type);

    void preLoadShaders();

private:
    std::unordered_map<ShaderType, GLitch::Shader> m_shaders;
};

#endif //SHADER_MANAGER_HPP
