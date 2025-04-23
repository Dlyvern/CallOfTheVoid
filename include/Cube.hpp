#ifndef CUBE_HPP
#define CUBE_HPP

#pragma once

#include "Texture.hpp"
#include "Shader.hpp"
#include "Common.hpp"
#include "GameObject.hpp"
#include "StaticModel.hpp"
#include "Physics.hpp"


#include "Material.hpp"

namespace geometries
{
    class Cube final : public GameObject
    {
    public:
        explicit Cube(const std::string& name);
        void setModel(StaticModel* model);
        void update(float deltaTime) override;
        void rotate(bool rotateClockwise);
        void setRigidBody(physx::PxRigidActor* rigidBody);
        physx::PxRigidActor* getRigidBody() const;

        void setPosition(const glm::vec3& position) override;
        void setScale(const glm::vec3& scale) override;

        Material getMaterial() const;

        StaticModel* getModel() const;

        void calculateShadows(Shader& shader) override;
        void setMaterial(const Material& material);
        void setMaterial(Material* material);

        Shader& getShader();

    private:
        bool m_rotate{false};

        glm::mat4 computeModelMatrix();

        Material m_material;

        physx::PxRigidActor* m_rigidBody{nullptr};
        StaticModel* m_model{nullptr};
        float m_rotationSpeed{50.0f};
        bool m_rotateClockwise{true};
    };
} //namespace geometries

#endif //CUBE_HPP