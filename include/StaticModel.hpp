#ifndef STATIC_MODEL_HPP
#define STATIC_MODEL_HPP
#include "StaticMesh.hpp"

class StaticModel final : public common::Model
{
public:
    StaticModel(const std::string& name, const std::vector<StaticMesh>& meshes);

    StaticModel();

    void draw() override;

    ModelType getType() override {return ModelType::STATIC;}

    std::vector<StaticMesh>& getMeshes();

    size_t getMeshesSize() override {return m_meshes.size();}

    common::Mesh* getMesh(int index) override
    {
        return &m_meshes[index];
    }

private:
    std::vector<StaticMesh> m_meshes;
};

#endif //STATIC_MODEL_HPP
