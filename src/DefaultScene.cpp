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
    // const std::string skyboxesFolder = filesystem::getSkyboxesFolderPath();
    // m_skybox.init({});
    // m_skybox.loadFromHDR(skyboxesFolder + "/nice.hdr");

    CameraManager::getInstance().getActiveCamera()->setCameraMode(CameraMode::FPS);

    this->setGameObjects(SceneManager::loadObjectsFromFile(filesystem::getMapsFolderPath().string() + "/test_scene.json"));

    Material mannequinMaterial;
    mannequinMaterial.addTexture(GLitch::Texture::TextureType::Diffuse, AssetsManager::instance().getTextureByName("Ch36_1001_Diffuse.png"));

    // auto mannequin = std::make_shared<Void>("mannequin");
    // mannequin->setMaterial(mannequinMaterial);
    // mannequin->setModel(AssetsManager::instance().getSkinnedModelByName("mannequin.fbx"));
    // mannequin->setPosition(glm::vec3(-5.0f, -0.5f, 0.0f));
    // mannequin->setScale(glm::vec3(0.015, 0.015, 0.015));
    // mannequin->setRigidBody(physics::PhysicsController::instance().addStaticActor(mannequin, true));
    // mannequin->playAnimation();
    // this->addGameObject(mannequin);

    // auto voidObject = std::make_shared<Void>("void");
    // voidObject->setMaterial(mannequinMaterial);
    // voidObject->setModel(AssetsManager::instance().getSkinnedModelByName("void.fbx"));
    // voidObject->setScale(glm::vec3{0.003f, 0.003f, 0.003f});
    // voidObject->setRotation(180, glm::vec3{0.0, 1.0, 0.0});
    // voidObject->setPosition({5.0f, 0.0f, 0.0f});
    // voidObject->setRigidBody(physics::PhysicsController::instance().addStaticActor(voidObject, true));
    // voidObject->playAnimation();
    // this->addGameObject(voidObject);


    m_player = std::make_shared<Player>("player");
    m_player->init({0.0f, 1.0f, 0.0f});
    this->addGameObject(m_player);

    lighting::Light directionalLight;
    directionalLight.type = lighting::LightType::DIRECTIONAL;
    directionalLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    directionalLight.position = glm::vec3(0.0f, 4.0f, 2.0f);
    directionalLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    directionalLight.strength = 2.0f;

    // lighting::Light secondLight;
    // secondLight.type = lighting::LightType::DIRECTIONAL;
    // secondLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    // secondLight.position = glm::vec3(4.0f, 4.0f, 2.0f);
    // secondLight.color = glm::vec3(255.0f, 0.0f, 0.0f);
    // secondLight.strength = 0.1f;

    auto lightObject = std::make_shared<geometries::Cube>("light");
    lightObject->setModel(AssetsManager::instance().getStaticModelByName("cube.obj"));
    lightObject->setPosition(directionalLight.position);
    lightObject->setScale({0.5f, 0.5f, 0.5f});
    lightObject->setRigidBody(physics::PhysicsController::instance().addStaticActor(lightObject));
    lightObject->addComponent<LightComponent>(directionalLight);

    // auto secondLightObject = std::make_shared<geometries::Cube>("second_light");
    // secondLightObject->setModel(AssetsManager::instance().getStaticModelByName("cube.obj"));
    // secondLightObject->setPosition(secondLight.position);
    // secondLightObject->setScale({0.5f, 0.5f, 0.5f});
    // secondLightObject->setRigidBody(physics::PhysicsController::instance().addStaticActor(secondLightObject));
    // secondLightObject->addComponent<LightComponent>(secondLight);

    LightManager::instance().addLight(lightObject->getComponent<LightComponent>()->getLight());
    // LightManager::instance().addLight(secondLightObject->getComponent<LightComponent>()->getLight());

    this->addGameObject(lightObject);
    // this->addGameObject(secondLightObject);

    m_shadowHandler.initShadows();
}

void calculateShadows(GLitch::Shader& shader, const std::vector<std::shared_ptr<GameObject>>& gameObjects, ShadowHandler& shadowHandler)
{
    static constexpr float nearPlane = 1.0f, farPlane = 7.5f;

    glm::vec3 lightPos = LightManager::instance().getLights()[0]->position;
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

    // m_skybox.render();

    debug::DebugTextHolder::instance().update(deltaTime);
}

bool DefaultScene::isOver()
{
    return false;
}

DefaultScene::~DefaultScene() = default;

