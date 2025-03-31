#include "Shader.hpp"
#include "Filesystem.hpp"

#include "../libraries/glad/glad.h"

#include <iostream>

namespace 
{
    int checkCompileErrors(unsigned int shader, const char* type)
    {
        int success;
        char infoLog[1024];

        if (type != "PROGRAM") 
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (!success) 
            {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);

            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog;
            }
        }

        return success;
    }
}

void Shader::load(const std::string &vertexPath, const std::string &fragmentPath,const char* geometryPath)
{
    std::string vertexSource = filesystem::getFileContents(vertexPath);
    std::string fragmentSource = filesystem::getFileContents(fragmentPath);

    const char* vertexCode = vertexSource.c_str();
    const char* fragmentCode = fragmentSource.c_str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    ::checkCompileErrors(vertex, "VERTEX");

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    ::checkCompileErrors(fragment, "FRAGMENT");

    int tempID = glCreateProgram();
    glAttachShader(tempID, vertex);
    glAttachShader(tempID, fragment);
    glLinkProgram(tempID);

    if (::checkCompileErrors(tempID, "PROGRAM")) 
    {
        if (m_id != -1) 
        {
            glDeleteProgram(m_id);
        }

        m_id = tempID;
	}
    else 
    {
		std::cout << "shader failed to compile " << vertexPath << " " << fragmentPath << "\n";
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::bind()
{
    glUseProgram(m_id);
}

int Shader::getId() const
{
    return m_id;
}

void Shader::setMat4(const std::string &name, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value)
{
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::setFloat(const std::string &name, float value)
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setInt(const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setUniform1i(const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}
