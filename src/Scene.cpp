#include "Scene.hpp"

Scene::Scene() = default;

Scene::~Scene() = default;

void Scene::setOnSceneEndCallback(const std::function<void(Scene* scene)> &callback)
{
    onEndSceneCallback_ = callback;
}

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
    if (!gameObject)
        return false;

    const auto it = std::find_if(m_objects.begin(), m_objects.end(), [&gameObject](const std::shared_ptr<GameObject>& gO)
    {
        return gO->getName() == gameObject->getName();
    });

    if (it == m_objects.end())
        return false;

    it->get()->destroy();
    m_objects.erase(it);
    
    return true;
}

const std::vector<std::shared_ptr<GameObject>>& Scene::getGameObjects()
{
    return m_objects;
}
