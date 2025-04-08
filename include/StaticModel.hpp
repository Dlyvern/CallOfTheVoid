#ifndef STATIC_MODEL_HPP
#define STATIC_MODEL_HPP
#include "StaticMesh.hpp"

class StaticModel final : public common::Model
{
public:
    StaticModel(const std::string& name, const std::vector<StaticMesh>& meshes);

    StaticModel();

    void draw() override;
private:
    std::vector<StaticMesh> m_meshes;
    std::string m_name{"undefined"};
};

#endif //STATIC_MODEL_HPP
