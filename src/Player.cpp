#include "Player.hpp"
#include "Keyboard.hpp"
#include "CameraManager.hpp"
#include "Cube.hpp"

struct UserCollision final : physx::PxUserControllerHitReport
{
    void onShapeHit(const physx::PxControllerShapeHit& hit) override {

    }
    void onControllerHit(const physx::PxControllersHit& hit) override {

    }
    void onObstacleHit(const physx::PxControllerObstacleHit& hit) override {

    }
} userCollision;

Player::Player(const std::string &name) : GameObject(name) 
{
    m_camera = CameraManager::getInstance().getActiveCamera();
}

void Player::init(const glm::vec3 &position)
{
    physx::PxMaterial* material = physics::PhysicsController::instance().getDefaultMaterial();
    auto* desc = new physx::PxCapsuleControllerDesc;
    desc->setToDefault();
    desc->height = HEIGHT;
    desc->radius = RADIUS;
    desc->position = physx::PxExtendedVec3(position.x, position.y + (HEIGHT / 2) + (RADIUS * 2), position.z);
    desc->material = material;
    desc->stepOffset = 0.05f;
    desc->contactOffset = 0.001;
    desc->scaleCoeff = .99f;
    desc->reportCallback = &userCollision;
    desc->slopeLimit = cosf(glm::radians(85.0f));

    m_controller = physics::PhysicsController::instance().getControllerManager()->createController(*desc);

    m_camera->setPosition(position + glm::vec3(0.0f, HEIGHT, 0.0f));
}

void Player::move(const glm::vec3 &direction, float deltaTime)
{
    physx::PxVec3 displacement(direction.x, direction.y, direction.z);
    physx::PxControllerFilters filters;
    m_controller->move(displacement * m_movementSpeed * deltaTime, 0.001f, deltaTime, filters);
}

void Player::update(float deltaTime)
{
    if (!m_isGrounded)
        m_velocity.y -= 9.81f * deltaTime;

    glm::vec3 moveDir(0.0f);

    glm::vec3 forward = m_camera->getForward();
    forward.y = 0.0f;
    forward = glm::normalize(forward);

    if (input::Keyboard.isKeyPressed(input::KeyCode::W))
        moveDir += forward;

    if (input::Keyboard.isKeyPressed(input::KeyCode::S))
        moveDir -= forward;

    if (input::Keyboard.isKeyPressed(input::KeyCode::A))
        moveDir -= glm::normalize(glm::cross(forward, m_camera->getUp()));

    if (input::Keyboard.isKeyPressed(input::KeyCode::D))
        moveDir += glm::normalize(glm::cross(forward, m_camera->getUp()));

    if (glm::length(moveDir) > 0.0f)
        moveDir = glm::normalize(moveDir) * m_movementSpeed * deltaTime;

    physx::PxVec3 displacement(moveDir.x, m_velocity.y * deltaTime, moveDir.z);
    physx::PxControllerCollisionFlags collisionFlags = m_controller->move(displacement, 0.01f, deltaTime, physx::PxControllerFilters());

    m_isGrounded = (collisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN);

    if (m_isGrounded)
        m_velocity.y = 0;

    if (m_isGrounded && input::Keyboard.isKeyPressed(input::KeyCode::SPACE))
    {
        m_velocity.y = m_jumpForce;
        m_isGrounded = false;
    }

    Player::setPosition(glm::vec3(m_controller->getFootPosition().x, m_controller->getFootPosition().y, m_controller->getFootPosition().z));
}

void Player::setPosition(const glm::vec3 &position)
{
    GameObject::setPosition(position);
    m_camera->setPosition(getPosition() + glm::vec3(0, HEIGHT, 0.0f));
}

// float velocity = m_movementSpeed * deltaTime;
// auto position = this->getPosition();
//
// if(input::Keyboard.isKeyPressed(input::KeyCode::W))
//     position += m_camera->getForward() * velocity;
//
// if(input::Keyboard.isKeyPressed(input::KeyCode::S))
//     position -= m_camera->getForward() * velocity;
//
// if(input::Keyboard.isKeyPressed(input::KeyCode::A))
//     position -= velocity * glm::normalize(glm::cross(m_camera->getForward(), m_camera->getUp()));
//
// if(input::Keyboard.isKeyPressed(input::KeyCode::D))
//     position += velocity * glm::normalize(glm::cross(m_camera->getForward(), m_camera->getUp()));
//
// this->setPosition(position);
