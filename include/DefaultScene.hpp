#ifndef DEFAULT_SCENE_HPP
#define DEFAULT_SCENE_HPP

#include "Scene.hpp"

#include "Cube.hpp"
#include "Player.hpp"

class DefaultScene final : public Scene
{
public:
    DefaultScene();

    void create() override;

    void update(float deltaTime) override;

    bool isOver() override;

    ~DefaultScene() override;

private:
    std::vector<std::shared_ptr<GameObject>> m_gameObjects;
    std::shared_ptr<geometries::Cube> m_man;
    std::shared_ptr<Player> m_player;
};


#endif //DEFAULT_SCENE_HPP