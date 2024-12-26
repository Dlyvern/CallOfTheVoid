#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/vec3.hpp>

namespace lighting
{
enum class LightType : uint8_t
{
    POINT = 0,
    DIRECTIONAL = 1,
    SPOT = 2
};

class Light
{
public:
    
private:
    glm::vec3 m_position{1.0f};
    glm::vec3 m_color{1, 0.7799999713897705, 0.5289999842643738};
    float m_strength{1.0f};
    float m_radius{6.0f};
    LightType m_type{LightType::POINT};
};
} //namespace lighting

#endif //LIGHT_HPP