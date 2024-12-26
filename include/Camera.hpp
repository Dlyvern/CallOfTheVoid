#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera
{
public:
    Camera();
    void update(float deltaTime);
    
    glm::vec3 getPosition() const;
    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    glm::mat4 getViewMatrix() const;
    float getPitch() const;

    void setPosition(const glm::vec3& position);

private:
    glm::vec3 m_position{glm::vec3(0.0f, 0.0f, 0.0f)};
    glm::vec3 m_up{glm::vec3(0.0f, 1.0f, 0.0f)};
    glm::vec3 m_right{glm::vec3(0.0f, 1.0f, 0.0f)};
    glm::vec3 m_forward{glm::vec3(0.0f, 0.0f, -1.0f)};

    float m_yaw{-90.0f};
    float m_pitch{0.0f};

    float m_mouseSensitivity{0.1f};
    bool m_firstClick{true};


    void updateCameraVectors();

};

#endif //CAMERA_HPPs