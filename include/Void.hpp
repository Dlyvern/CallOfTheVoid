#ifndef VOID_HPP
#define VOID_HPP

#include "Texture.hpp"
#include "Shader.hpp"
#include "Common.hpp"
#include "GameObject.hpp"
#include "Model.hpp"

#include "AnimatedGameObject.hpp"

class Void : public GameObject
{
public:
    explicit Void(const std::string& name);

    void create();
    void setTexture(textures::Texture* texture);
    void setModel(Model* model);
    void update(const glm::mat4& viewMatrix, const glm::vec3& cameraPosition, float deltaTime) override;
    void setRotation(float angle, const glm::vec3& axis);
    void rotate(bool rotateClockwise);

    ~Void() override;
private:
    bool m_rotate{false};

    AnimatedGameObject* m_animatedGameObject;

    textures::Texture* m_texture{nullptr};
    Model* m_model{nullptr};
    Shader m_shader;
    glm::vec3 m_rotation{0.0f};
    float m_rotationSpeed{50.0f};
    bool m_rotateClockwise{true};
};

#endif //VOID_HPP