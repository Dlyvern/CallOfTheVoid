#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GameObject.hpp"
#include "Camera.hpp"
#include "Physics.hpp"

class Player final : public GameObject
{
public:
    explicit Player(const std::string& name);

    void init(const glm::vec3& position);

    void update(float deltaTime) override;

    void setPosition(const glm::vec3& position) override;

    void calculateShadows(Shader& shader) override{}

private:
    void move(const glm::vec3& direction, float deltaTime);
    void interact();

    Camera* m_camera{nullptr};
    float m_movementSpeed{3.0f};

    physx::PxController* m_controller{nullptr};

    static constexpr float RADIUS{0.20f};
    static constexpr float HEIGHT{1.75f};

    const glm::vec3 m_cameraOffset{0.0f, HEIGHT, 0.0f};
    glm::vec3 m_velocity{0.0f, 0.0f, 0.0f};
    float m_jumpForce{5.0f};
    bool m_isGrounded{false};
};



#endif //PLAYER_HPP
