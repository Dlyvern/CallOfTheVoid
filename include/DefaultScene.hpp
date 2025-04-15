#ifndef DEFAULT_SCENE_HPP
#define DEFAULT_SCENE_HPP

#include "Scene.hpp"
#include "Player.hpp"
#include "Skybox.hpp"

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
    std::shared_ptr<Player> m_player;
    Skybox m_skybox;
};

#endif //DEFAULT_SCENE_HPP