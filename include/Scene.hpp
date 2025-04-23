#ifndef SCENE_HPP
#define SCENE_HPP
#include <vector>

#include "GameObject.hpp"
#include <memory>

class Scene
{
public:
    Scene();

    virtual ~Scene();

    virtual void update(float deltaTime) = 0;

    virtual void create() = 0;

    virtual bool isOver() = 0;

    void addGameObject(std::shared_ptr<GameObject> gameObject);

    const std::vector<std::shared_ptr<GameObject>>& getGameObjects();

private:
    std::vector<std::shared_ptr<GameObject>> m_objects;
};

#endif //SCENE_HPP