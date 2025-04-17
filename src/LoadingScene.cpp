#include "LoadingScene.hpp"
#include "Filesystem.hpp"
#include "AssetsManager.hpp"

LoadingScene::LoadingScene() = default;

void LoadingScene::create()
{
    AssetsManager::instance().preLoadModels({"Sitting_Angry.fbx","Mannequin.fbx", "cube.obj", "man.fbx", "mannequin.fbx", "Jail_Door.fbx", "void.fbx"});

    const std::string shadersDirectory = filesystem::getShadersFolderPath().string();
    const std::string fontName = filesystem::getFontsFolderPath().string() + "/ghostmane_font.ttf";

    m_text.initShader(shadersDirectory + "/text.vert", shadersDirectory + "/text.frag");
    m_text.setText("LOADING");
    m_text.setFont(fontName);

    m_future = std::async(std::launch::async, [this] { loadAllAssets(); });
}

void LoadingScene::loadAllAssets()
{
    const std::vector<std::string> modelsPaths{
        "void.fbx", "blender_cube.obj",
        "mannequin.fbx", "man.fbx"
    };

    const std::vector<std::string> texturesPaths{
        "void.png", "wooden_floor.png",
        "man_Packed0_Diffuse.png", "wallParking_d.png",
        "concrete_d.png", "default_texture.png",

        "MannequinDiffuse.png", "MannequinNormal.png",
        "MannequinSpecular.png", "MannequinAo.png",

        "older-wood-flooring_albedo.png",
        "older-wood-flooring_ao.png", "older-wood-flooring_height.png",
        "older-wood-flooring_metallic.png", "older-wood-flooring_roughness.png"

    };

    AssetsManager::instance().preLoadModels(modelsPaths);
    AssetsManager::instance().preLoadTextures(texturesPaths);
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