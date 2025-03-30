// #include "Bone.hpp"
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/quaternion.hpp>
//
// Bone::Bone(const std::string& name, int id, const std::vector<KeyPosition>& positions, const std::vector<KeyRotation>& rotations, const std::vector<KeyScale>& scales)
// : m_Name(name), m_ID(id), m_Positions(positions), m_Rotations(rotations), m_Scales(scales) {}
//
// void Bone::update(float animationTime) {
//     glm::vec3 position = interpolatePosition(animationTime);
//     glm::quat rotation = interpolateRotation(animationTime);
//     glm::vec3 scale = interpolateScale(animationTime);
//
//     m_FinalTransformation = glm::translate(glm::mat4(1.0f), position) *
//                             glm::mat4_cast(rotation) *
//                             glm::scale(glm::mat4(1.0f), scale);
// }
//
// int Bone::getPositionIndex(float animationTime) {
//     for (int i = 0; i < m_Positions.size() - 1; ++i) {
//         if (animationTime < m_Positions[i + 1].timeStamp) return i;
//     }
//     return m_Positions.size() - 1;
// }
//
// glm::vec3 Bone::interpolatePosition(float animationTime) {
//     if (m_Positions.size() == 1) return m_Positions[0].position;
//
//     int index = getPositionIndex(animationTime);
//     int nextIndex = index + 1;
//
//     float deltaTime = m_Positions[nextIndex].timeStamp - m_Positions[index].timeStamp;
//     float factor = (animationTime - m_Positions[index].timeStamp) / deltaTime;
//
//     return glm::mix(m_Positions[index].position, m_Positions[nextIndex].position, factor);
// }