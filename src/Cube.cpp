#include "Cube.hpp"
#include "Renderer.hpp"
#include "ShaderManager.hpp"

geometries::Cube::Cube(const std::string &name) : GameObject(name)
{
    GameObject::setLayerMask(common::LayerMask::DEFAULT);
}

void geometries::Cube::calculateShadows(GLitch::Shader& shader)
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
    this->updateComponents(deltaTime);

    const auto shader = ShaderManager::instance().getShader(ShaderManager::ShaderType::STATIC);
    shader->bind();

    const auto&[viewMatrix, projectionMatrix, cameraPosition] = Renderer::instance().getFrameData();
    shader->setMat4("model", computeModelMatrix());
    shader->setMat4("view", viewMatrix);
    shader->setMat4("projection", projectionMatrix);
    shader->setVec3("viewPos", cameraPosition);

    m_model->draw();
}

void geometries::Cube::destroy()
{
    GameObject::destroy();

    if (m_rigidBody)
    {
        m_rigidBody->release();
        m_rigidBody = nullptr;
    }
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
}

StaticModel* geometries::Cube::getModel() const
{
    return m_model;
}
