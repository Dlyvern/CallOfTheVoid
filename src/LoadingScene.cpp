#include "LoadingScene.hpp"
#include "Filesystem.hpp"
#include "AssetsManager.hpp"

LoadingScene::LoadingScene() = default;

void LoadingScene::create()
{
    AssetsManager::instance().preLoadModels({ "VendingMachine.fbx", "Arms_Mak.fbx", "cube.obj", "mannequin.fbx", "jail_door.fbx", "void.fbx"});
    const std::string fontName = filesystem::getFontsFolderPath().string() + "/ghostmane_font.ttf";
    m_text.setText("LOADING");
    m_text.setFont(fontName);

    m_future = std::async(std::launch::async, [this] { loadAllAssets(); });
}

void LoadingScene::loadAllAssets()
{
    const std::vector<std::string> modelsPaths{
        // "void.fbx",
        // "mannequin.fbx",
    };

    const std::vector<std::string> texturesPaths{
        "Ch36_1001_Diffuse.png", "wall_parking_d.png",
        "concrete_d.png", "wall_parking_n.png",

        "MannequinDiffuse.png", "MannequinNormal.png",
        "MannequinSpecular.png", "MannequinAo.png",

        "wood_floor_d.png", "wood_floor_n.png",
        "wood_floor_ao.png", "older-wood-flooring_height.png",
        "wood_floor_m.png", "wood_floor_r.png",

        "armCcolor.png", "armCNormal.png", "armcRoughness.png",
        "makarov_attachments_Base_Color.png",
        "makarov_attachments_Emissive.png", "makarov_attachments_Metallic-makarov_attac.png",
        "makarov_attachments_Normal_OpenGL.png", "Makarov_main_Base_Color.png",
        "Makarov_main_Metallic_png-Makarov_main_Rou.png", "Makarov_main_Normal_OpenGL.png"
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