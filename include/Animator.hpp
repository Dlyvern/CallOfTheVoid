#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#include <cmath>
#include <glm/gtx/string_cast.hpp>

#include "Interpolation.hpp"

class Animator
{
public:
	Animator();

	void updateAnimation(float deltaTime);

	void playAnimation(common::Animation* animation, bool repeat = true);

	void calculateBoneTransform(common::BoneInfo* boneInfo, const glm::mat4 &parentTransform, common::Animation* animation, float currentTime);

	bool isAnimationPlaying() const;

	std::vector<glm::mat4> getFinalBoneMatrices();

private:
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

	std::vector<glm::mat4> m_finalBoneMatrices;
};

#endif