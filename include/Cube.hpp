#ifndef CUBE_HPP
#define CUBE_HPP

#pragma once

#include "Texture.hpp"
#include "Shader.hpp"
#include "Common.hpp"
#include "GameObject.hpp"
#include "StaticModel.hpp"
#include "Physics.hpp"

namespace geometries
{
    class Cube : public GameObject
    {
    public:
        explicit Cube(const std::string& name);
        void create();
        void setTexture(textures::Texture* texture);
        void setModel(StaticModel* model);
        void update(float deltaTime) override;
        void setRotation(float angle, const glm::vec3& axis);
        void rotate(bool rotateClockwise);
        void setRigidBody(physx::PxRigidActor* rigidBody);
    private:
        bool m_rotate{false};

        physx::PxRigidActor* m_rigidBody{nullptr};

        textures::Texture* m_texture{nullptr};
        StaticModel* m_model{nullptr};
        Shader m_shader;
        glm::vec3 m_rotation{0.0f};
        float m_rotationSpeed{50.0f};
        bool m_rotateClockwise{true};
    };

} //namespace geometries

#endif //CUBE_HPP