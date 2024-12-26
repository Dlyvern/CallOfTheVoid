#include "Void.hpp"

#include "Filesystem.hpp"

Void::Void(const std::string &name) : GameObject(name)
{
}

void Void::create()
{
    auto shadersPath = filesystem::getShadersFolderPath().string();

    m_shader.load(shadersPath + "/skelet.vert", shadersPath + "/skelet.frag");
}

void Void::setTexture(textures::Texture *texture)
{
    m_texture = texture;
}

void Void::setModel(Model *model)
{
    m_model = model;
    m_animatedGameObject = new AnimatedGameObject(model);

    m_animatedGameObject->playAnimation(&m_model->getAnimations()[0], 1);
}

void Void::update(const glm::mat4 &viewMatrix, const glm::vec3 &cameraPosition, float deltaTime)
{
    m_animatedGameObject->updateAnimation(deltaTime);

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

    for (unsigned int i = 0; i < m_model->boneTransforms.size(); ++i) 
    {
        std::string uniformName = "finalBonesMatrices[" + std::to_string(i) + "]";
        m_shader.setMat4(uniformName.c_str(), m_model->boneTransforms[i]);
    }

    model = glm::translate(model, getPosition());
    model = glm::scale(model, getScale());
    model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Вращение вокруг оси X
    model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Вращение вокруг оси Y
    model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Вращение вокруг оси Z
    projection = glm::perspective(glm::radians(45.0f), (float)1920 / 1080, 0.1f, 100.0f);

    m_shader.setMat4("model", model);
    m_shader.setMat4("view", viewMatrix);
    m_shader.setMat4("projection", projection);
    // m_shader.setVec3("viewPos", cameraPosition);
    // m_shader.setVec3("light.position", glm::vec3{1.0f, 1.0f, 1.0f});
    // m_shader.setVec3("light.color", glm::vec3{1.0f, 1.0f, 1.0f});
    // m_shader.setFloat("light.strength", 0.3f);
    // m_shader.setFloat("light.radius", 6.0f);

    m_texture->bind(0);

    m_shader.setUniform1i("texture_diffuse1", 0);

    m_model->draw();
}

void Void::setRotation(float angle, const glm::vec3 &axis)
{
    if (axis == glm::vec3(1.0f, 0.0f, 0.0f))
        m_rotation.x += angle;
    else if (axis == glm::vec3(0.0f, 1.0f, 0.0f))
        m_rotation.y += angle;
    else if (axis == glm::vec3(0.0f, 0.0f, 1.0f))
        m_rotation.z += angle;
}

void Void::rotate(bool rotateClockwise)
{
    m_rotate = true;
    m_rotateClockwise = rotateClockwise;
}

Void::~Void() = default;
