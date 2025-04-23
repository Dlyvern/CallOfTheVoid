#include "LoadingScene.hpp"
#include "Filesystem.hpp"
#include "AssetsManager.hpp"

LoadingScene::LoadingScene() = default;

void LoadingScene::create()
{
    AssetsManager::instance().preLoadModels({"Mannequin.fbx", "cube.obj", "mannequin.fbx", "jail_door.fbx", "void.fbx"});
    const std::string fontName = filesystem::getFontsFolderPath().string() + "/ghostmane_font.ttf";
    m_text.setText("LOADING");
    m_text.setFont(fontName);

    m_future = std::async(std::launch::async, [this] { loadAllAssets(); });
}

void LoadingScene::loadAllAssets()
{
    const std::vector<std::string> modelsPaths{
        "void.fbx",
        "mannequin.fbx",
    };

    const std::vector<std::string> texturesPaths{
        "void.png", "wall_parking_d.png",
        "concrete_d.png", "wall_parking_n.png",

        "MannequinDiffuse.png", "MannequinNormal.png",
        "MannequinSpecular.png", "MannequinAo.png",

        "wood_floor_d.png", "wood_floor_n.png",
        "wood_floor_ao.png", "older-wood-flooring_height.png",
        "wood_floor_m.png", "wood_floor_r.png"
    };

    const std::vector<std::string> materialsPaths
    {
        "default_material.mat", "floor_material.mat", "parking_wall_material.mat"
    };

    AssetsManager::instance().preLoadModels(modelsPaths);
    AssetsManager::instance().preLoadTextures(texturesPaths);
    AssetsManager::instance().preLoadMaterials(materialsPaths);
}

void LoadingScene::update(float deltaTime)
{
    m_text.draw();

    if(m_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
        m_endScene = true;
}

bool LoadingScene::isOver()
{
    return m_endScene;
}

LoadingScene::~LoadingScene() = default;