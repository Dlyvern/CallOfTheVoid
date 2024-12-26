#include "Player.hpp"
#include "Keyboard.hpp"

Player::Player(const std::string &name) : GameObject(name) 
{
    this->setScale({0.3f, 1.8f, 0.5f}); 

    m_camera = std::make_shared<Camera>();
}

void Player::update(const glm::mat4& viewMatrix, const glm::vec3& cameraPosition, float deltaTime)
{
    float velocity = m_movementSpeed * deltaTime;
    auto position = this->getPosition();

    if(input::Keyboard.isKeyPressed(input::KeyCode::W))
        position += m_camera->getForward() * velocity;
    
    if(input::Keyboard.isKeyPressed(input::KeyCode::S))
        position -= m_camera->getForward() * velocity;

    if(input::Keyboard.isKeyPressed(input::KeyCode::A))
        position -= velocity * glm::normalize(glm::cross(m_camera->getForward(), m_camera->getUp()));

    if(input::Keyboard.isKeyPressed(input::KeyCode::D))
        position += velocity * glm::normalize(glm::cross(m_camera->getForward(), m_camera->getUp()));

    this->setPosition(position);

    m_camera->update(deltaTime);
}

void Player::setPosition(const glm::vec3 &position)
{
    GameObject::setPosition(position);
    m_camera->setPosition(getPosition());
}

const std::shared_ptr<Camera> Player::getCamera() const
{
    return m_camera;
}
