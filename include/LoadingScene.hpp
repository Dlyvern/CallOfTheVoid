#ifndef LOADING_SCENE_HPP
#define LOADING_SCENE_HPP

#include "Scene.hpp"
#include <future>

#include "Text.hpp"

class LoadingScene final : public Scene
{
public:
    LoadingScene();

    void create() override;

    void update(float deltaTime) override;

    bool isOver() override;

    ~LoadingScene() override;

private:
    void loadAllAssets();
    std::future<void> m_future;
    bool m_endScene{false};

    Text m_text;
};


#endif //LOADING_SCENE_HPP