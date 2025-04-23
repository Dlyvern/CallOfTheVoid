#ifndef DEFAULT_SCENE_HPP
#define DEFAULT_SCENE_HPP

#include "Scene.hpp"
#include "Player.hpp"
#include "Skybox.hpp"
#include "ShadowHandler.hpp"

class DefaultScene final : public Scene
{
public:
    DefaultScene();

    void create() override;

    void update(float deltaTime) override;

    bool isOver() override;

    ~DefaultScene() override;
private:
    ShadowHandler m_shadowHandler;
    std::shared_ptr<Player> m_player;
    Skybox m_skybox;
};

#endif //DEFAULT_SCENE_HPP