#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GameObject.hpp"
#include "Camera.hpp"
#include <memory>

class Player : public GameObject
{
public:
    explicit Player(const std::string& name);

    void update(const glm::mat4& viewMatrix, const glm::vec3& cameraPosition, float deltaTime) override;

    void setPosition(const glm::vec3& position) override;

    const std::shared_ptr<Camera> getCamera() const;

    
private:
    std::shared_ptr<Camera> m_camera{nullptr};
    float m_movementSpeed{2.5f};
};

#endif //PLAYER_HPP