#ifndef SCENE_HPP
#define SCENE_HPP

#include "GameObject.hpp"

class Scene
{
public:
    Scene();

    virtual ~Scene();

    virtual void update(float deltaTime) = 0;

    virtual void create() = 0;

    virtual bool isOver() = 0;

    void setOnSceneEndCallback(const std::function<void(Scene* scene)>& callback);

    void addGameObject(std::shared_ptr<GameObject> gameObject);

    void setGameObjects(const std::vector<std::shared_ptr<GameObject>>& gameObjects);

    bool deleteGameObject(GameObject* gameObject);

    const std::vector<std::shared_ptr<GameObject>>& getGameObjects();

protected:
    std::function<void(Scene* scene)> onEndSceneCallback_;

private:
    std::vector<std::shared_ptr<GameObject>> m_objects;
};

#endif //SCENE_HPP