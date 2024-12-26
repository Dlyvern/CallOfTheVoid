#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "Common.hpp"

class Animation
{
public:
    void setName(const std::string& name);
    void setDuration(const float duration);
    void setTicksPerSecond(const float ticksPerSecond);
    void setBoneAnimations(const std::vector<common::BoneAnimation>& boneAnimations);


    inline std::string getName() const {return m_name;}
    inline float getTicksPerSecond() const {return m_ticksPerSecond;}
    inline float getDuration() const {return m_duration;}
    inline std::vector<common::BoneAnimation> getBoneAnimations() {return m_boneAnimations;}
private:
    std::string m_name;
    float m_ticksPerSecond;
    float m_duration;
    std::vector<common::BoneAnimation> m_boneAnimations;
};


#endif //ANIMATION_HPP