#include "DefaultScene.hpp"
#include "CameraManager.hpp"
#include "Filesystem.hpp"
#include "Renderer.hpp"
#include "SceneManager.hpp"
#include "Player.hpp"

DefaultScene::DefaultScene() = default;

void DefaultScene::create()
{
    CameraManager::getInstance().getActiveCamera()->setCameraMode(CameraMode::FPS);
    Renderer::instance().initShadows();
    this->setGameObjects(SceneManager::loadObjectsFromFile(filesystem::getMapsFolderPath().string() + "/test_scene.json"));

    m_skybox.init({});
    m_skybox.loadFromHDR(filesystem::getSkyboxesFolderPath().string() + "/nice.hdr");

    auto player = std::make_shared<Player>("player");
    player->init({0.0f, 1.0f, 0.0f});
    this->addGameObject(player);
}

void DefaultScene::update(float deltaTime)
{
    for(auto& gameObject : this->getGameObjects())
        gameObject->update(deltaTime);

    // m_skybox.render();
}

bool DefaultScene::isOver()
{
    return false;
}

DefaultScene::~DefaultScene() = default;
