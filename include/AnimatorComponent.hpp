#ifndef ANIMATOR_COMPONENT_HPP
#define ANIMATOR_COMPONENT_HPP

#include "Component.hpp"
#include "Common.hpp"

class AnimatorComponent final : public Component
{
public:
    AnimatorComponent();

    void update(float deltaTime) override;

    void playAnimation(common::Animation* animation, bool repeat = true);

    void stopAnimation();

    [[nodiscard]] bool isAnimationPlaying() const;
private:
    void calculateBoneTransform(common::BoneInfo* boneInfo, const glm::mat4 &parentTransform, common::Animation* animation, float currentTime);
    void calculateObjectTransform(common::Animation* animation, float currentTime);

    bool m_isAnimationPaused{false};
    bool m_isAnimationLooped{true};
    bool m_isAnimationCompleted{false};
    bool m_isInterpolating{false};

    float m_animationSpeed{1.0f};
    float m_currentTime{0.0f};
    float m_haltTime{0.0f};
    float m_interTime{0.0f};

    common::Animation* m_currentAnimation{nullptr};
    common::Animation* m_nextAnimation{nullptr};
    common::Animation* m_queueAnimation{nullptr};
};

#endif //ANIMATOR_COMPONENT_HPP
