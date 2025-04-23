#include "DefaultScene.hpp"
#include "AssetsManager.hpp"
#include "Cube.hpp"
#include "CameraManager.hpp"
#include "Filesystem.hpp"
#include "Light.hpp"
#include "LightManager.hpp"
#include "Physics.hpp"
#include "DebugTextHolder.hpp"
#include "LightComponent.hpp"
#include "SceneManager.hpp"
#include "ShaderManager.hpp"
#include "Void.hpp"

DefaultScene::DefaultScene() = default;

void DefaultScene::create()
{
    const std::string skyboxesFolder = filesystem::getSkyboxesFolderPath();
    m_skybox.init({});
    m_skybox.loadFromHDR(skyboxesFolder + "/nice.hdr");

    const std::vector<std::string> neededTexturesNames{
        "wall_parking_d.png", "wall_parking_n.png", "concrete_d.png", "void.png", "MannequinDiffuse.png",
        "MannequinNormal.png", "MannequinSpecular.png", "MannequinAo.png", "wood_floor_d.png", "wood_floor_n.png",
        "wood_floor_ao.png", "older-wood-flooring_height.png", "wood_floor_m.png", "wood_floor_r.png"
    };

    for (const auto& textureName : neededTexturesNames)
        if (auto texture = AssetsManager::instance().getTextureByName(textureName); texture && !texture->isBaked())
            texture->bake();

    Material mannequinMaterial;
    mannequinMaterial.addTexture(textures::TextureType::Diffuse, AssetsManager::instance().getTextureByName("void.png"));

    auto mannequin = std::make_shared<Void>("mannequin");
    mannequin->setMaterial(mannequinMaterial);
    mannequin->setModel(AssetsManager::instance().getSkinnedModelByName("mannequin.fbx"));
    mannequin->setPosition(glm::vec3(-5.0f, -0.5f, 0.0f));
    mannequin->setScale(glm::vec3(0.015, 0.015, 0.015));
    mannequin->setRigidBody(physics::PhysicsController::instance().addStaticActor(mannequin, true));
    mannequin->playAnimation();
    this->addGameObject(mannequin);

    // auto voidObject = std::make_shared<Void>("void");
    // voidObject->setMaterial(mannequinMaterial);
    // voidObject->setModel(AssetsManager::instance().getSkinnedModelByName("void.fbx"));
    // voidObject->setScale(glm::vec3{0.003f, 0.003f, 0.003f});
    // voidObject->setRotation(180, glm::vec3{0.0, 1.0, 0.0});
    // voidObject->setPosition({5.0f, 0.0f, 0.0f});
    // voidObject->setRigidBody(physics::PhysicsController::instance().addStaticActor(voidObject, true));
    // voidObject->playAnimation();
    // this->addGameObject(voidObject);

    CameraManager::getInstance().getActiveCamera()->setCameraMode(CameraMode::FPS);

    for (const auto& ob : SceneManager::loadObjectsFromFile(filesystem::getMapsFolderPath().string() + "/default_scene.json"))
        this->addGameObject(ob);

    m_player = std::make_shared<Player>("player");
    m_player->init({0.0f, 1.0f, 0.0f});
    this->addGameObject(m_player);

    lighting::Light directionalLight;
    directionalLight.type = lighting::LightType::DIRECTIONAL;
    directionalLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    directionalLight.position = glm::vec3(0.0f, 4.0f, 2.0f);
    directionalLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    directionalLight.strength = 2.0f;

    auto lightObject = std::make_shared<geometries::Cube>("light");
    lightObject->setModel(AssetsManager::instance().getStaticModelByName("cube.obj"));
    lightObject->setPosition(directionalLight.position);
    lightObject->setScale({0.5f, 0.5f, 0.5f});
    lightObject->setRigidBody(physics::PhysicsController::instance().addStaticActor(lightObject));
    lightObject->addComponent<LightComponent>(directionalLight);

    LightManager::instance().addLight(lightObject->getComponent<LightComponent>()->getLight());


    this->addGameObject(lightObject);

    m_shadowHandler.initShadows();
}

void calculateShadows(Shader& shader, const std::vector<std::shared_ptr<GameObject>>& gameObjects, ShadowHandler& shadowHandler)
{
    static constexpr float nearPlane = 1.0f, farPlane = 7.5f;

    glm::vec3 lightPos = LightManager::instance().getLights()[0].position;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    LightManager::instance().setLightSpaceMatrix(lightSpaceMatrix);

    shader.bind();
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    shadowHandler.bindShadows();

    for (const auto& obj : gameObjects)
        obj->calculateShadows(shader);

    shadowHandler.unbindShadows();
}

void DefaultScene::update(float deltaTime)
{
    calculateShadows(*ShaderManager::instance().getShader(ShaderManager::ShaderType::STATIC_SHADOW), this->getGameObjects(), m_shadowHandler);
    calculateShadows(*ShaderManager::instance().getShader(ShaderManager::ShaderType::SKELETON_SHADOW), this->getGameObjects(), m_shadowHandler);

    m_shadowHandler.activateTexture();

    for(auto& gameObject : this->getGameObjects())
        gameObject->update(deltaTime);

    auto skeletonShader = ShaderManager::instance().getShader(ShaderManager::ShaderType::SKELETON);
    auto staticShader = ShaderManager::instance().getShader(ShaderManager::ShaderType::STATIC);

    LightManager::instance().bindGlobalLighting(*skeletonShader);
    LightManager::instance().bindGlobalLighting(*staticShader);

    m_shadowHandler.deactivateTexture();

    m_skybox.render();

    debug::DebugTextHolder::instance().update(deltaTime);
}

bool DefaultScene::isOver()
{
    return false;
}

DefaultScene::~DefaultScene() = default;

