#include "AnimatedGameObject.hpp"
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <iostream>

glm::vec3 interpolate(const glm::vec3& start, const glm::vec3& end, float t) {
    return start + t * (end - start);
}

glm::quat interpolate(const glm::quat& start, const glm::quat& end, float t) {
    return glm::slerp(start, end, t);
}

float interpolate(float start, float end, float t) {
    return start + t * (end - start);
}

std::pair<const common::SQT*, const common::SQT*> findKeyframes(const std::vector<common::SQT>& keyFrames, float currentTime)
{
    for (size_t i = 1; i < keyFrames.size(); ++i) 
        if (currentTime < keyFrames[i].timeStamp)
            return {&keyFrames[i - 1], &keyFrames[i]};

    return {&keyFrames[keyFrames.size() - 2], &keyFrames[keyFrames.size() - 1]};
}

AnimatedGameObject::AnimatedGameObject(Model *model)
{
    m_model = model;
}

void AnimatedGameObject::playAnimation(Animation *animation, float speed)
{
    m_currentAnimation = animation;
    m_animationSpeed = speed;

    std::cout << "Playing " << m_currentAnimation->getName() << " animation" << std::endl;
}

void AnimatedGameObject::updateAnimation(float deltaTime)
{
    if(!m_currentAnimation)
        return;
    
    float duration = m_currentAnimation->getDuration() / m_currentAnimation->getTicksPerSecond();

    if (!m_isAnimationPaused)
        m_currentTime += deltaTime * m_animationSpeed;

    // m_currentTime = std::fmod(m_currentTime, duration); // Essential for looping

    // if (m_currentTime < 0.0f) {
    //     m_currentTime += duration;
    // }

    // if (m_currentTime > duration && !m_isAnimationLooped)
    // {
    //     m_currentTime = duration;
    //     m_isAnimationPaused = true;
    //     m_isAnimationCompleted = true;
    // }

    if (m_currentTime > duration)
    {
        if (!m_isAnimationLooped)
        {
            m_currentTime = duration;
            m_isAnimationPaused = true;
            m_isAnimationCompleted = true;
        }
        else
            m_currentTime = 0;
    }

    calculateBoneTransforms();

}

void AnimatedGameObject::calculateBoneTransforms()
{
    auto bones = m_model->getBonesInfo();
    auto bonesAnimations = m_model->getAnimations()[0].getBoneAnimations();

    if (bonesAnimations.empty()) 
    {
        std::cerr << "AnimatedGameObject::calculateBoneTransforms(): No bone animations found" << std::endl;
        return;
    }

    for (const auto& boneAnim : bonesAnimations) 
    {
        auto [startFrame, endFrame] = findKeyframes(boneAnim.keyFrames, m_currentTime);

        float t = (m_currentTime - startFrame->timeStamp) / (endFrame->timeStamp - startFrame->timeStamp);

        glm::vec3 position = interpolate(startFrame->position, endFrame->position, t);
        glm::quat rotation = interpolate(startFrame->rotation, endFrame->rotation, t);
        float scale = interpolate(startFrame->scale, endFrame->scale, t);

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                                glm::mat4(rotation) *
                                glm::scale(glm::mat4(1.0f), glm::vec3(scale));

        std::string jointName = startFrame->jointName;
        auto boneMap = m_model->getBoneMap();

        int boneIndex = boneMap[jointName];

        transform = transform * bones[boneIndex].offsetMatrix;

        bones[boneIndex].finalTransformation = transform;
        m_model->boneTransforms[boneIndex] = transform;
    }

    m_model->setBonesInfo(bones);
}

        // auto startFrameIt = std::lower_bound(boneAnim.keyFrames.begin(), boneAnim.keyFrames.end(), m_currentTime, [](const common::SQT& a, float time){ return a.timeStamp < time; });

        // // Handle edge case, where current time is *beforethe first keyframe
        // if (startFrameIt == boneAnim.keyFrames.begin())
        //     startFrameIt = boneAnim.keyFrames.end() - 1;
        // else
        //     startFrameIt--; 


        // auto endFrameIt = startFrameIt + 1; // Calculate the next keyframe, wrapping around if needed
        // if (endFrameIt == boneAnim.keyFrames.end())
        //     endFrameIt = boneAnim.keyFrames.begin();

        // // Important: Check if the keyframes are valid before use
        // if (startFrameIt == boneAnim.keyFrames.end() || endFrameIt == boneAnim.keyFrames.end()) {
        //     std::cerr << "Error: Invalid keyframes for bone animation.\n";
        //     continue; // Skip to next bone animation
        // }

        // float t = (m_currentTime - startFrameIt->timeStamp) / (endFrameIt->timeStamp - startFrameIt->timeStamp);

        // if(t<0) t = 0;
        // if(t>1) t = 1;
