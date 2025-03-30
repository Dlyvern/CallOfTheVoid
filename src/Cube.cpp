#include "Cube.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AssetsManager.hpp"
#include "Filesystem.hpp"

#include "CameraManager.hpp"

geometries::Cube::Cube(const std::string &name) : GameObject(name)
{
}

void geometries::Cube::create()
{
    // m_model = AssetsManager::instance().loadModel("../resources/man.obj");
    auto shadersPath = filesystem::getShadersFolderPath().string();

    m_shader.load(shadersPath + "/cube.vert", shadersPath + "/cube.frag");
}

void geometries::Cube::setTexture(textures::Texture *texture)
{
    m_texture = texture;
}

void geometries::Cube::setModel(Model *model)
{
    m_model = model;
}

void geometries::Cube::update(float deltaTime)
{
    m_shader.bind();

    if(m_rotate)
    {
        if (m_rotateClockwise)
            m_rotation += m_rotationSpeed * deltaTime;
        else
            m_rotation -= m_rotationSpeed * deltaTime;

        // m_rotation = fmod(m_rotation, 360.0f);
        m_rotate = false;
    }

    glm::mat4 model(1.0f);
    glm::mat4 projection(1.0f);

    model = glm::translate(model, getPosition());
    model = glm::scale(model, getScale());
    model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Вращение вокруг оси X
    model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Вращение вокруг оси Y
    model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Вращение вокруг оси Z
    projection = glm::perspective(glm::radians(45.0f), (float)1920 / 1080, 0.1f, 100.0f);

    m_shader.setMat4("model", model);
    m_shader.setMat4("view", CameraManager::getInstance().getActiveCamera()->getViewMatrix());
    m_shader.setMat4("projection", projection);
    m_shader.setVec3("viewPos", CameraManager::getInstance().getActiveCamera()->getPosition());
    m_shader.setVec3("light.position", glm::vec3{1.0f, 1.0f, 1.0f});
    m_shader.setVec3("light.color", glm::vec3{1.0f, 1.0f, 1.0f});
    m_shader.setFloat("light.strength", 0.3f);
    m_shader.setFloat("light.radius", 6.0f);

    m_texture->bind(0);

    m_shader.setUniform1i("tex0", 0);

    m_model->draw();
}

void geometries::Cube::rotate(bool rotateClockwise)
{
    m_rotate = true;
    m_rotateClockwise = rotateClockwise;
}

void geometries::Cube::setRotation(float angle, const glm::vec3& axis)
{
    if (axis == glm::vec3(1.0f, 0.0f, 0.0f))
        m_rotation.x += angle;
    else if (axis == glm::vec3(0.0f, 1.0f, 0.0f))
        m_rotation.y += angle;
    else if (axis == glm::vec3(0.0f, 0.0f, 1.0f))
        m_rotation.z += angle;
}