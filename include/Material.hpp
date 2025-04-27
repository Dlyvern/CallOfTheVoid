#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <Texture.hpp>

#include "Texture.hpp"
#include <unordered_map>
#include "Shader.hpp"

class Material
{
public:
    Material();

    explicit Material(const std::string& name);

    std::string getName() const;

    void setName(const std::string& name);

    void setBaseColor(const glm::vec3& color);

    void addTexture(const GLitch::Texture::TextureType& type, GLitch::Texture* texture);

    GLitch::Texture* getTexture(const GLitch::Texture::TextureType& type);

    const glm::vec3& getBaseColor() const;

    void bind(GLitch::Shader& shader);
private:
    std::string m_name{"Undefined"};
    std::unordered_map<GLitch::Texture::TextureType, GLitch::Texture*> m_textures;
    glm::vec3 m_baseColor = glm::vec3(128, 128, 128);
    bool m_useBaseColor = true;
};

#endif //MATERIAL_HPP
