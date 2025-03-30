#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GameObject.hpp"
#include "Camera.hpp"
#include <memory>

class Player final : public GameObject
{
public:
    explicit Player(const std::string& name);

    void update(float deltaTime) override;

    void setPosition(const glm::vec3& position) override;

    Camera* getCamera() const;

    
private:
    Camera* m_camera{nullptr};
    float m_movementSpeed{2.5f};
};

#endif //PLAYER_HPP