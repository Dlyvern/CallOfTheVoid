#ifndef CUBE_HPP
#define CUBE_HPP

#include "GameObject.hpp"
#include "StaticModel.hpp"
#include "Physics.hpp"

namespace geometries
{
    class Cube final : public GameObject
    {
    public:
        explicit Cube(const std::string& name);

        void update(float deltaTime) override;
        void destroy() override;
        void calculateShadows(GLitch::Shader& shader) override;

        void setModel(StaticModel* model);
        void setRigidBody(physx::PxRigidActor* rigidBody);
        void setPosition(const glm::vec3& position) override;
        void setScale(const glm::vec3& scale) override;

        physx::PxRigidActor* getRigidBody() const;
        StaticModel* getModel() const override;



    private:
        glm::mat4 computeModelMatrix();

        physx::PxRigidActor* m_rigidBody{nullptr};
        StaticModel* m_model{nullptr};
    };

} //namespace geometries

#endif //CUBE_HPP