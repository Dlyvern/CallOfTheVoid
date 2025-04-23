#include "Scene.hpp"

Scene::Scene() = default;

Scene::~Scene() = default;

void Scene::addGameObject(std::shared_ptr<GameObject> gameObject)
{
    m_objects.push_back(gameObject);
}

const std::vector<std::shared_ptr<GameObject>>& Scene::getGameObjects()
{
    return m_objects;
}
