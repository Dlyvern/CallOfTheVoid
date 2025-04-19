#include "Camera.hpp"
#include "Mouse.hpp"
#include "DebugEditor.hpp"
#include "WindowsManager.hpp"
#include "DebugTextHolder.hpp"

unsigned int textIndex{0};

Camera::Camera()
{
    updateCameraVectors();

    textIndex = debug::DebugTextHolder::instance().addText("Camera position " + std::to_string(m_position.x) + " " + std::to_string(m_position.y) + " " + std::to_string(m_position.z));
}

void Camera::update(float deltaTime)
{
    if (m_mode == CameraMode::Static)
    {
        updateCameraVectors();
        return;
    }

    static bool mouseLocked{false};

    auto* window = window::WindowsManager::instance().getCurrentWindow();

    if (DebugEditor::instance().getDebugMode())
    {
        if (input::Mouse.isRightButtonPressed() && !mouseLocked)
        {
            glfwSetInputMode(window->getOpenGLWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_firstClick = true;
            mouseLocked = true;
        }
        else if((!input::Mouse.isRightButtonPressed()) && mouseLocked)
        {
            glfwSetInputMode(window->getOpenGLWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseLocked = false;
        }

        if (!mouseLocked)
        {
            updateCameraVectors();
            return;
        }
    }

    static float lastX = static_cast<float>(window->getWidth()) / 2.0f;
    static float lastY = static_cast<float>(window->getHeight()) / 2.0f;

    const auto xPosition = static_cast<float>(input::Mouse.getX());
    const auto yPosition = static_cast<float>(input::Mouse.getY());

    if(m_firstClick)
    {
        lastX = xPosition;
        lastY = yPosition;
        m_firstClick = false;
    }

    float offsetX = xPosition - lastX;
    float offsetY = yPosition - lastY;

    lastX = xPosition;
    lastY = yPosition;

    offsetX *= m_mouseSensitivity;
    offsetY *= m_mouseSensitivity;

    m_yaw += offsetX;
    m_pitch -= offsetY;

    // if (m_yaw > 180.0f)  m_yaw -= 360.0f;
    // if (m_yaw < -180.0f) m_yaw += 360.0f;

    m_pitch = glm::clamp(m_pitch - offsetY, -89.0f, 89.0f);

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

    debug::DebugTextHolder::instance().changeText(textIndex, "Camera position " + std::to_string(m_position.x) + " " + std::to_string(m_position.y) + " " + std::to_string(m_position.z));
}

void Camera::setCameraMode(const CameraMode &mode)
{
    m_mode = mode;
}
