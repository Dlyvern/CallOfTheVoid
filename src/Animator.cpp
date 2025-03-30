#include "Animator.hpp"

Animator::Animator()
{
    m_finalBoneMatrices.reserve(100);

    for (int i = 0; i < 100; i++)
        m_finalBoneMatrices.emplace_back(1.0f);
}

void Animator::updateAnimation(float deltaTime)
{
    if (!m_currentAnimation)
        return;

    m_currentTime = std::fmod(m_currentTime + m_currentAnimation->ticksPerSecond * deltaTime, m_currentAnimation->duration);

    if (m_isInterpolating)
    {
        if (m_queueAnimation)
        {
            m_currentAnimation = m_nextAnimation;
            m_haltTime = 0.0f;
            m_nextAnimation = m_queueAnimation;
            m_queueAnimation = nullptr;
            m_currentTime = 0.0f;
            m_interTime = 0.0;
            return;
        }

        m_isInterpolating = false;
        m_currentAnimation = m_nextAnimation;
        m_currentTime = 0.0;
        m_interTime = 0.0;
    }

    auto parent = m_currentAnimation->findParent();
    calculateBoneTransform(parent, glm::mat4(1.0f), m_currentAnimation, m_currentTime);
}

void Animator::playAnimation(common::Animation *animation, const bool repeat)
{
    m_isAnimationLooped = repeat;

    if (!m_currentAnimation) {
        m_currentAnimation = animation;
        return;
    }

    if (m_isInterpolating) {
        if (animation != m_nextAnimation)
            m_queueAnimation = animation;
    }
    else {
        if (animation != m_nextAnimation) {
            m_isInterpolating = true;
            m_haltTime = fmod(m_currentTime, m_currentAnimation->duration);
            m_nextAnimation = animation;
            m_currentTime = 0.0f;
            m_interTime = 0.0;
        }
    }
}

void Animator::calculateBoneTransform(common::BoneInfo *boneInfo, const glm::mat4 &parentTransform, common::Animation *animation, const float currentTime)
{
    std::string nodeName = boneInfo->name;
    glm::mat4 boneTransform = boneInfo->offsetMatrix;

    if (common::BoneAnimation* boneAnimation = animation->getBoneAnimation(nodeName))
    {
        auto [startFrame, endFrame] = findKeyframes(boneAnimation->keyFrames, currentTime);

        if (!startFrame || !endFrame)
            return;

        float deltaTime = endFrame->timeStamp - startFrame->timeStamp;
        float t = (deltaTime == 0) ? 0.0f : (currentTime - startFrame->timeStamp) / deltaTime;
        t = glm::clamp(t, 0.0f, 1.0f);

        glm::vec3 position = interpolate(startFrame->position, endFrame->position, t);
        glm::quat rotation = glm::normalize(glm::slerp(startFrame->rotation, endFrame->rotation, t));
        glm::vec3 scale = interpolate(startFrame->scale, endFrame->scale, t);

        boneTransform = glm::translate(glm::mat4(1.0f), position) *
                    glm::toMat4(rotation) *
                    glm::scale(glm::mat4(1.0f), scale);
    }

    glm::mat4 globalTransformation = parentTransform * boneTransform;

    auto boneProps = animation->modelBones;

    for (unsigned int i = 0; i < boneProps->size(); i++)
    {
        if (boneProps->at(i).name == nodeName)
        {
            glm::mat4 offset = boneProps->at(i).offsetMatrix;
            m_finalBoneMatrices[i] = globalTransformation * offset;
            break;
        }
    }

    for (auto i : boneInfo->children) {
        auto child = animation->findBone(i);
        calculateBoneTransform(child, globalTransformation, animation, currentTime);
    }
}

std::vector<glm::mat4> Animator::getFinalBoneMatrices()
{
    return m_finalBoneMatrices;
}
