#include "Cube.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "AssetsManager.hpp"
#include "Filesystem.hpp"
#include "CameraManager.hpp"
#include "WindowsManager.hpp"
#include "LightManager.hpp"

geometries::Cube::Cube(const std::string &name) : GameObject(name)
{
    setLayerMask(common::LayerMask::DEFAULT);
}

void geometries::Cube::create()
{
    const std::string shadersPath = filesystem::getShadersFolderPath().string();

    m_shader.load(shadersPath + "/cube.vert", shadersPath + "/cube.frag");
}

void geometries::Cube::calculateShadows(Shader& shader)
{
    shader.setMat4("model", computeModelMatrix());

    m_model->draw();
}

glm::mat4 geometries::Cube::computeModelMatrix()
{
    glm::mat4 model = glm::mat4(1.0f);

    if (m_rigidBody)
    {
        const physx::PxTransform transform = m_rigidBody->getGlobalPose();

        this->setPosition({transform.p.x, transform.p.y, transform.p.z});

        model = glm::translate(model, glm::vec3(transform.p.x, transform.p.y, transform.p.z));
    }
    else
        model = glm::translate(model, getPosition());

    model = glm::scale(model, getScale());
    model = glm::rotate(model, glm::radians(getRotation().x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(getRotation().y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(getRotation().z), glm::vec3(0, 0, 1));

    return model;
}

void geometries::Cube::update(float deltaTime)
{
    m_shader.bind();

    // if(m_rotate)
    // {
    //     m_rotateClockwise ? m_rotation += m_rotationSpeed * deltaTime : m_rotation -= m_rotationSpeed * deltaTime;
    //     m_rotate = false;
    // }

    const auto* currentWindow = window::WindowsManager::instance().getCurrentWindow();
    const float aspect = static_cast<float>(currentWindow->getWidth()) / static_cast<float>(currentWindow->getHeight());

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    m_shader.setMat4("model", computeModelMatrix());
    m_shader.setMat4("view", CameraManager::getInstance().getActiveCamera()->getViewMatrix());
    m_shader.setMat4("projection", projection);
    m_shader.setVec3("viewPos", CameraManager::getInstance().getActiveCamera()->getPosition());
    m_shader.setMat4("lightSpaceMatrix", LightManager::instance().getLightSpaceMatrix());

    LightManager::instance().sendLightsIntoShader(m_shader);

    m_material.bind(m_shader);

    m_shader.setInt("shadowMap", 1);

    m_model->draw();
}

void geometries::Cube::rotate(const bool rotateClockwise)
{
    m_rotate = true;
    m_rotateClockwise = rotateClockwise;
}

void geometries::Cube::setModel(StaticModel* model)
{
    m_model = model;
}

void geometries::Cube::setRigidBody(physx::PxRigidActor* rigidBody)
{
    m_rigidBody = rigidBody;
}

physx::PxRigidActor*geometries::Cube::getRigidBody() const
{
    return m_rigidBody;
}

void geometries::Cube::setPosition(const glm::vec3 &position)
{
    GameObject::setPosition(position);

    if (m_rigidBody)
        m_rigidBody->setGlobalPose({position.x, position.y, position.z});
}

void geometries::Cube::setScale(const glm::vec3 &scale)
{
    GameObject::setScale(scale);
    // m_rigidBody.get
}

void geometries::Cube::setMaterial(const Material &material)
{
    m_material = material;
}
