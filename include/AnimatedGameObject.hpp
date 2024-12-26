#ifndef ANIMATED_GAME_OBJECT_HPP
#define ANIMATED_GAME_OBJECT_HPP

#include "Model.hpp"
#include "Animation.hpp"

class AnimatedGameObject
{
public:
    AnimatedGameObject(Model* model);

    void playAnimation(Animation* animation, float speed);

    void updateAnimation(float deltaTime);

    void calculateBoneTransforms();

private:
    Model* m_model{nullptr};

    float m_currentTime{0};
    float m_animationSpeed{0};

    Animation* m_currentAnimation{nullptr};

    bool m_isAnimationPaused{false};
    bool m_isAnimationLooped{true};
    bool m_isAnimationCompleted{false};

};

#endif //ANIMATED_GAME_OBJECT_HPP