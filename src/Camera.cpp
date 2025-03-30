#include "Camera.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include <glm/ext/matrix_transform.hpp>

Camera::Camera()
{
    updateCameraVectors();
}

void Camera::update(float deltaTime)
{
    if (m_mode == CameraMode::Static) {
        updateCameraVectors();
        return;
    }
    static float lastX = 1920.0f / 2.0f;
    static float lastY = 1080.0f / 2.0f;

    if(m_firstClick)
    {
        lastX = input::Mouse.getX();
        lastY = input::Mouse.getY();
        m_firstClick = false;
    }

    float offsetX = input::Mouse.getX() - lastX;
    float offsetY = input::Mouse.getY() - lastY; // Corrected to match typical mouse coordinate system
    
    lastX = input::Mouse.getX();
    lastY = input::Mouse.getY();

    offsetX *= m_mouseSensitivity;
    offsetY *= m_mouseSensitivity;

    m_yaw += offsetX;
    
    //For now, our camera can move only right and left. Uncomment to fix it

    if (m_yaw > 360.0f) m_yaw -= 360.0f;
    if (m_yaw < 0.0f) m_yaw += 360.0f;

    m_pitch -= offsetY;

    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 forward;

    forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    forward.y = sin(glm::radians(m_pitch));
    forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward = glm::normalize(forward);

    m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::normalize(glm::cross(m_right, m_forward));
}

glm::vec3 Camera::getPosition() const
{
    return m_position;
}

glm::vec3 Camera::getForward() const
{
    return m_forward;
}

glm::vec3 Camera::getUp() const
{
    return m_up;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_forward, m_up);
}

float Camera::getPitch() const
{
    return m_pitch;
}

void Camera::setPosition(const glm::vec3 &position)
{
    m_position = position;
}

void Camera::setCameraMode(const CameraMode &mode)
{
    m_mode = mode;
}
