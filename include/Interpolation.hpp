#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

#include "Common.hpp"
#include <glm/gtx/quaternion.hpp>
inline glm::vec3 interpolate(const glm::vec3& start, const glm::vec3& end, float t) {
    return start + t * (end - start);
}

inline glm::quat interpolate(const glm::quat& start, const glm::quat& end, float t) {
    return glm::slerp(start, end, t);
}

inline float interpolate(float start, float end, float t) {
    return start + t * (end - start);
}

inline std::pair<const common::SQT*, const common::SQT*> findKeyframes(const std::vector<common::SQT>& keyFrames, const float currentTime)
{
    if (keyFrames.empty())
        return {nullptr, nullptr};

    if (currentTime <= keyFrames.front().timeStamp)
        return {&keyFrames.front(), &keyFrames.front()};

    if (currentTime >= keyFrames.back().timeStamp)
        return {&keyFrames.back(), &keyFrames.back()};

    for (size_t i = 1; i < keyFrames.size(); ++i)
        if (currentTime < keyFrames[i].timeStamp)
            return {&keyFrames[i - 1], &keyFrames[i]};

    return {nullptr, nullptr};  // Should never reach here
}

#endif