#ifndef CUBE_HPP
#define CUBE_HPP

#pragma once

#include "Texture.hpp"
#include "Shader.hpp"
#include "Common.hpp"
#include "GameObject.hpp"
#include "Model.hpp"

namespace geometries
{
    class Cube : public GameObject
    {
    public:
        explicit Cube(const std::string& name);
        void create();
        void setTexture(textures::Texture* texture);
        void setModel(Model* model);
        void update(const glm::mat4& viewMatrix, const glm::vec3& cameraPosition, float deltaTime) override;
        void setRotation(float angle, const glm::vec3& axis);
        void rotate(bool rotateClockwise);
    private:
        bool m_rotate{false};

        textures::Texture* m_texture{nullptr};
        Model* m_model{nullptr};
        Shader m_shader;
        glm::vec3 m_rotation{0.0f};
        float m_rotationSpeed{50.0f};
        bool m_rotateClockwise{true};
    };

} //namespace geometries

#endif //CUBE_HPP