#include "Scene.hpp"

Scene::Scene() = default;

Scene::~Scene() = default;

void Scene::addGameObject(std::shared_ptr<GameObject> gameObject)
{
    m_objects.push_back(gameObject);
}

void Scene::setGameObjects(const std::vector<std::shared_ptr<GameObject>> &gameObjects)
{
    m_objects = gameObjects;
}

bool Scene::deleteGameObject(GameObject *gameObject)
{
    auto it = std::find_if(m_objects.begin(), m_objects.end(), [&gameObject](std::shared_ptr<GameObject> gO) {
        return gO->getName() == gameObject->getName();
    });

    if (it != m_objects.end())
    {
        it->get()->destroy();
        m_objects.erase(it);
        return true;
    }

    return false;
}

const std::vector<std::shared_ptr<GameObject>>& Scene::getGameObjects()
{
    return m_objects;
}
