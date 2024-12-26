#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
    void load(const std::string& vertexPath, const std::string& fragmentPath, const char* geometryPath = nullptr);
    void bind();

    int getId() const;

    void setMat4(const std::string& name, const glm::mat4& value);

    void setVec3(const std::string& name, const glm::vec3& value);

    void setFloat(const std::string& name, float value);

    void setInt(const std::string& name, int value);

    void setUniform1i(const std::string& name, int value);

private:
    int m_id{-1};
};

#endif //SHADER_HPP