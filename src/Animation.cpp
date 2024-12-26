#include "Animation.hpp"

void Animation::setName(const std::string &name)
{
    m_name = name;
}

void Animation::setDuration(const float duration)
{
    m_duration = duration;
}

void Animation::setTicksPerSecond(const float ticksPerSecond)
{
    m_ticksPerSecond = ticksPerSecond;
}

void Animation::setBoneAnimations(const std::vector<common::BoneAnimation> &boneAnimations)
{
    m_boneAnimations = boneAnimations;
}
