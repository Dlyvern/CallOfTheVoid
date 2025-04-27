#ifndef STATIC_MODEL_HPP
#define STATIC_MODEL_HPP
#include "StaticMesh.hpp"

class StaticModel final : public common::Model
{
public:
    StaticModel(const std::string& name, const std::vector<StaticMesh>& meshes);

    StaticModel();

    void draw() override;

    std::string getName() {return m_name;}

    ModelType getType() override {return ModelType::STATIC;}

    std::vector<StaticMesh>& getMeshes();
private:
    std::vector<StaticMesh> m_meshes;
    std::string m_name{"undefined"};
};

#endif //STATIC_MODEL_HPP
