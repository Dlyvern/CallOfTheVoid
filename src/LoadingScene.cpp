#include "LoadingScene.hpp"
#include "Filesystem.hpp"
#include <glm/glm.hpp>
#include "Texture.hpp"
#include "AssetsManager.hpp"
#include <future>

#include "CameraManager.hpp"
#include "Cube.hpp"
#include "Light.hpp"
#include "LightManager.hpp"
#include "Void.hpp"

#include "Physics.hpp"

namespace
{
    std::vector<std::shared_ptr<GameObject>> gameObjects;
}

LoadingScene::LoadingScene() : Scene()
{

}

void LoadingScene::create()
{
    AssetsManager::instance().preLoadModels({"void.fbx", "cube.obj"});
    AssetsManager::instance().preLoadTextures({"void.png", "web.png"});

    AssetsManager::instance().getTextureByName("void.png")->bake();
    AssetsManager::instance().getTextureByName("web.png")->bake();

    // auto voidObject = std::make_shared<Void>("Void");
    // voidObject->create();

    // voidObject->setTexture(AssetsManager::instance().getTextureByName("void.png"));
    // voidObject->setModel(AssetsManager::instance().getSkinnedModelByName("void.fbx"));
    // voidObject->setScale(glm::vec3{0.01f, 0.01f, 0.01f});
    // voidObject->setRotation(180, glm::vec3{0.0, 1.0, 0.0});
    // voidObject->setPosition({5.0f, -5.0f, -20.0f});
    // voidObject->playAnimation();
    // gameObjects.push_back(voidObject);


    // auto plane = std::make_shared<geometries::Cube>("Plane");
    // plane->create();
    // plane->setTexture(AssetsManager::instance().getTextureByName("web.png"));
    // plane->setModel(AssetsManager::instance().getStaticModelByName("cube.obj"));
    // plane->setPosition({0.0, 0.0, -30.0});
    // plane->setScale({10.0f, 10.0f, 10.0f});
    // gameObjects.push_back(plane);


    const auto shadersDirectory = filesystem::getShadersFolderPath();
    const std::string font_name = filesystem::getFontsFolderPath().string() + "/ghostmane_font.ttf";

    m_text.initShader(shadersDirectory.string() + "/text.vert", shadersDirectory.string() + "/text.frag");
    m_text.setText("LOADING");
    m_text.setFont(font_name);

    m_future = std::async(std::launch::async, [this] {
        loadAllAssets();
    });

    lighting::Light pointLight;
    pointLight.position = glm::vec3(0.0f, 10.0f, 0.0f);
    pointLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLight.strength = 0.01;
    LightManager::instance().addLight(pointLight);
}

void LoadingScene::loadAllAssets()
{
    AssetsManager::instance().preLoadModels({"blender_cube.obj", "man.fbx"});
    AssetsManager::instance().preLoadTextures({"wooden_floor.png", "man_Packed0_Diffuse.png", "wallParking_d.png", "concrete_d.png", "man_Packed0_Diffuse.png"});

    // auto texturesFuture = std::async(std::launch::async, []{AssetsManager::instance().loadTextures();});
    // auto modelsFuture = std::async(std::launch::async, []{AssetsManager::instance().loadModels();});
    // AssetsManager::instance().loadTextures();
    // AssetsManager::instance().loadModels();
    //
    // while(true)
    //     if(texturesFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready && modelsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    //         break;
}


void LoadingScene::update(float deltaTime)
{
    m_text.draw();

    for (const auto& gameObject : gameObjects)
        gameObject->update(deltaTime);

    // voidObject.rotate(true);

    if(m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        m_endScene = true;
}

bool LoadingScene::isOver()
{
    return m_endScene;
    // return false;
}

LoadingScene::~LoadingScene() = default;