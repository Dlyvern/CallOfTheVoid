#include "LoadingScene.hpp"
#include "Filesystem.hpp"
#include <glm/glm.hpp>
#include "Texture.hpp"
#include "AssetsManager.hpp"
#include <future>

#include "CameraManager.hpp"
#include "Cube.hpp"
#include "Void.hpp"

namespace
{
    textures::Texture* texture;
    Void voidObject("Void");
    geometries::Cube cube("cube");
}

LoadingScene::LoadingScene() : Scene()
{
}

void LoadingScene::create()
{
    texture = AssetsManager::instance().getTextureByName("void.png");
    texture->bake();

    cube.create();
    cube.setTexture(texture);
    cube.setModel(AssetsManager::instance().loadStaticModel("/home/dlyvern/Projects/CallOfTheVoid/resources/models/cube.obj"));
    cube.setPosition({1.1, -1.1, -20.1});
    cube.setScale({5.0f, 5.0f, 5.0f});
    // cube.setRotation(0, {});

    voidObject.create();

    voidObject.setTexture(texture);
    voidObject.setModel(AssetsManager::instance().getModelByName("void.fbx"));
    voidObject.setScale(glm::vec3{0.01f, 0.01f, 0.01f});
    voidObject.setRotation(180, glm::vec3{0.0, 1.0, 0.0});
    voidObject.setPosition({5.0f, -5.0f, -20.0f});

    auto shadersDirectory = filesystem::getShadersFolderPath();
    std::string font_name = filesystem::getFontsFolderPath().string() + "/ghostmane_font.ttf";

    m_text.initShader(shadersDirectory.string() + "/text.vert", shadersDirectory.string() + "/text.frag");
    m_text.setText("LOADING");
    m_text.setFont(font_name);

    // m_future = std::async(std::launch::async, [this]{loadAllAssets();});
}

void LoadingScene::loadAllAssets()
{
    auto texturesFuture = std::async(std::launch::async, []{AssetsManager::instance().loadTextures();});
    auto modelsFuture = std::async(std::launch::async, []{AssetsManager::instance().loadModels();});
    AssetsManager::instance().loadTextures();
    AssetsManager::instance().loadModels();

    while(true)
        if(texturesFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready && modelsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            break;
}


void LoadingScene::update(float deltaTime)
{
    CameraManager::getInstance().getActiveCamera()->update(deltaTime);

    m_text.draw();

    voidObject.update(deltaTime);
    cube.update(deltaTime);
    // voidObject.rotate(true);

    // if(m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    //     m_endScene = true;
}

bool LoadingScene::isOver()
{
    // return m_endScene;
    return false;
}

LoadingScene::~LoadingScene() = default;