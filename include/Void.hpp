#ifndef VOID_HPP
#define VOID_HPP

#include "Texture.hpp"
#include "Shader.hpp"
#include "Common.hpp"
#include "GameObject.hpp"
#include "SkinnedModel.hpp"

#include "Animator.hpp"


class Void : public GameObject
{
public:
    explicit Void(const std::string& name);

    void create();
    void setTexture(textures::Texture* texture);
    void setModel(SkinnedModel* model);
    void update(float deltaTime) override;
    void setRotation(float angle, const glm::vec3& axis);
    void rotate(bool rotateClockwise);
    void playAnimation();

    ~Void() override;
private:
    bool m_rotate{false};

    // AnimatedGameObject* m_animatedGameObject;

    common::Animation* m_animation{nullptr};

    Animator m_animator;

    textures::Texture* m_texture{nullptr};
    SkinnedModel* m_model{nullptr};
    Shader m_shader;
    glm::vec3 m_rotation{0.0f};
    float m_rotationSpeed{50.0f};
    bool m_rotateClockwise{true};
};

#endif //VOID_HPP