#include "AnimatorComponent.hpp"

#include <iostream>

#include "GameObject.hpp"
#include "Utilities.hpp"
#include "Skeleton.hpp"

AnimatorComponent::AnimatorComponent() = default;

void AnimatorComponent::update(float deltaTime)
{
    if (!m_currentAnimation)
        return;

    m_currentTime += m_currentAnimation->ticksPerSecond * deltaTime;

    if (m_isAnimationLooped)
        m_currentTime = std::fmod(m_currentTime + m_currentAnimation->ticksPerSecond * deltaTime, m_currentAnimation->duration);
    else if (m_currentTime > m_currentAnimation->duration)
    {
        m_currentTime = m_currentAnimation->duration;
        m_currentAnimation = nullptr;
        return;
    }

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

    if (m_currentAnimation->skeletonForAnimation)
    {
        const auto parent = m_currentAnimation->skeletonForAnimation->getParent();
        calculateBoneTransform(parent, glm::mat4(1.0f), m_currentAnimation, m_currentTime);
    }
    else if (m_currentAnimation->gameObject)
    {
        calculateObjectTransform(m_currentAnimation, m_currentTime);
    }
    //TODO Should we use it here or no? If we have no game object and skeleton to work with
    // else
    //     m_currentAnimation = nullptr;
}

void AnimatorComponent::calculateObjectTransform(common::Animation *animation, float currentTime)
{
    if (auto track = animation->getAnimationTrack("door"))
    {
        auto [startFrame, endFrame] = utilities::findKeyframes(track->keyFrames, currentTime);

        if (!startFrame || !endFrame)
            return;

        float deltaTime = endFrame->timeStamp - startFrame->timeStamp;
        float t = (deltaTime == 0) ? 0.0f : (currentTime - startFrame->timeStamp) / deltaTime;
        t = glm::clamp(t, 0.0f, 1.0f);

        glm::vec3 position = utilities::interpolate(startFrame->position, endFrame->position, t);
        glm::quat rotation = glm::normalize(glm::slerp(startFrame->rotation, endFrame->rotation, t));
        glm::vec3 scale = utilities::interpolate(startFrame->scale, endFrame->scale, t);

        auto* gameObject = animation->gameObject;

        position += glm::vec3{0.0f, gameObject->getPosition().y, gameObject->getPosition().z};

        glm::quat currentGameObjectRotationInQuaternion = glm::quat(glm::radians(gameObject->getRotation()));

        rotation = glm::normalize(rotation * currentGameObjectRotationInQuaternion);

        glm::vec3 eulerDegrees = glm::degrees(glm::eulerAngles(rotation));

        std::cout << eulerDegrees.x << " " << eulerDegrees.y << " " << eulerDegrees.z << std::endl;

        gameObject->setPosition(position);
        gameObject->setScale(scale);
        gameObject->setRotation(eulerDegrees);

        // glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
        //             glm::toMat4(rotation) *
        //             glm::scale(glm::mat4(1.0f), scale);
        //
        // glm::mat4 finalTransform = transform * gameObject->getTransformMatrix();
        //
        // gameObject->setTransformMatrix(transform);
    }
}


void AnimatorComponent::playAnimation(common::Animation *animation, const bool repeat)
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

void AnimatorComponent::stopAnimation()
{
    m_currentAnimation = nullptr;
}

void AnimatorComponent::calculateBoneTransform(common::BoneInfo *boneInfo, const glm::mat4 &parentTransform, common::Animation *animation, const float currentTime)
{
    std::string nodeName = boneInfo->name;
    glm::mat4 boneTransform = boneInfo->offsetMatrix;

    if (const auto* boneAnimation = animation->getAnimationTrack(nodeName))
    {
        auto [startFrame, endFrame] = utilities::findKeyframes(boneAnimation->keyFrames, currentTime);

        if (!startFrame || !endFrame)
            return;

        float deltaTime = endFrame->timeStamp - startFrame->timeStamp;
        float t = (deltaTime == 0) ? 0.0f : (currentTime - startFrame->timeStamp) / deltaTime;
        t = glm::clamp(t, 0.0f, 1.0f);

        const glm::vec3 position = utilities::interpolate(startFrame->position, endFrame->position, t);
        const glm::quat rotation = glm::normalize(glm::slerp(startFrame->rotation, endFrame->rotation, t));
        const glm::vec3 scale = utilities::interpolate(startFrame->scale, endFrame->scale, t);

        boneTransform = glm::translate(glm::mat4(1.0f), position) *
                    glm::toMat4(rotation) *
                    glm::scale(glm::mat4(1.0f), scale);
    }

    glm::mat4 globalTransformation = parentTransform * boneTransform;

    auto* skeleton = animation->skeletonForAnimation;

    if (const auto bone = skeleton->getBone(nodeName))
    {
        glm::mat4 offset = bone->offsetMatrix;
        boneInfo->finalTransformation = globalTransformation * offset;
    }

    for (const auto& i : boneInfo->children)
    {
        const auto child = skeleton->getBone(i);
        calculateBoneTransform(child, globalTransformation, animation, currentTime);
    }
}

bool AnimatorComponent::isAnimationPlaying() const
{
    return m_currentAnimation != nullptr;
}
