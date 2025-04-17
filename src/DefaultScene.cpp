#include "DefaultScene.hpp"
#include "AssetsManager.hpp"
#include "Raycasting.hpp"
#include "../libraries/json/json.hpp"
#include <fstream>
#include <glad.h>
#include "Cube.hpp"

#include "CameraManager.hpp"
#include "Filesystem.hpp"
#include "Light.hpp"
#include "LightManager.hpp"
#include "Physics.hpp"
#include "DebugLine.hpp"
#include "WindowsManager.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "DebugTextHolder.hpp"
#include "Void.hpp"
GLuint depthMapFBO;
GLuint depthMap;
const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
float nearPlane = 1.0f, farPlane = 7.5f;



// Shader shader;


DefaultScene::DefaultScene() = default;

void DefaultScene::create()
{
    const std::string textureFolder = filesystem::getTexturesFolderPath().string();

    m_skybox.init({
        textureFolder + "/right.jpg",
        textureFolder + "/left.jpg",
        textureFolder + "/top.jpg",
        textureFolder + "/bottom.jpg",
        textureFolder + "/front.jpg",
        textureFolder + "/back.jpg"
    });

    // //TODO FIX IT LATER, Bake textures after creating?
    AssetsManager::instance().getTextureByName("wooden_floor.png")->bake();
    AssetsManager::instance().getTextureByName("man_Packed0_Diffuse.png")->bake();
    AssetsManager::instance().getTextureByName("wallParking_d.png")->bake();
    AssetsManager::instance().getTextureByName("concrete_d.png")->bake();
    AssetsManager::instance().getTextureByName("void.png")->bake();
    AssetsManager::instance().getTextureByName("default_texture.png")->bake();

    AssetsManager::instance().getTextureByName("MannequinDiffuse.png")->bake();
    AssetsManager::instance().getTextureByName("MannequinNormal.png")->bake();
    AssetsManager::instance().getTextureByName("MannequinSpecular.png")->bake();
    AssetsManager::instance().getTextureByName("MannequinAo.png")->bake();

    AssetsManager::instance().getTextureByName("older-wood-flooring_albedo.png")->bake();
    AssetsManager::instance().getTextureByName("older-wood-flooring_ao.png")->bake();
    AssetsManager::instance().getTextureByName("older-wood-flooring_height.png")->bake();
    AssetsManager::instance().getTextureByName("older-wood-flooring_metallic.png")->bake();
    AssetsManager::instance().getTextureByName("older-wood-flooring_roughness.png")->bake();

    Material doorMaterial;
    doorMaterial.addTexture(textures::TextureType::Diffuse, AssetsManager::instance().getTextureByName("concrete_d.png"));
    // auto door = std::make_shared<geometries::Cube>("door");
    // door->create();
    // door->setMaterial(doorMaterial);
    // door->setModel(AssetsManager::instance().getStaticModelByName("Jail_Door.fbx"));
    // door->setRotation(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    // door->setPosition({1.0, 0.0, 0.0});
    // door->setScale({1.0f, 1.0f, 1.0f});
    // door->setRigidBody(physics::PhysicsController::instance().addStaticActor(door));

    Material mannequinMaterial;
    mannequinMaterial.addTexture(textures::TextureType::Diffuse, AssetsManager::instance().getTextureByName("MannequinDiffuse.png"));
    // mannequinMaterial.addTexture(textures::TextureType::Normal, AssetsManager::instance().getTextureByName("MannequinNormal.png"));
    // mannequinMaterial.addTexture(textures::TextureType::Specular, AssetsManager::instance().getTextureByName("MannequinSpecular.png"));
    // mannequinMaterial.addTexture(textures::TextureType::AO, AssetsManager::instance().getTextureByName("MannequinAo.png"));

    auto mannequin = std::make_shared<Void>("mannequin");
    mannequin->create();
    mannequin->setMaterial(mannequinMaterial);
    mannequin->setModel(AssetsManager::instance().getSkinnedModelByName("Mannequin.fbx"));
    mannequin->setPosition(glm::vec3(-5.0f, 0.0f, 0.0f));
    mannequin->setScale(glm::vec3(0.01, 0.01, 0.01));
    // mannequin->playAnimation();

    // Material planeMaterial;
    // planeMaterial.addTexture(textures::TextureType::Diffuse, AssetsManager::instance().getTextureByName("older-wood-flooring_albedo.png"));
    // planeMaterial.addTexture(textures::TextureType::AO, AssetsManager::instance().getTextureByName("older-wood-flooring_ao.png"));
    // planeMaterial.addTexture(textures::TextureType::Height, AssetsManager::instance().getTextureByName("older-wood-flooring_height.png"));
    // planeMaterial.addTexture(textures::TextureType::Metallic, AssetsManager::instance().getTextureByName("older-wood-flooring_metallic.png"));
    // planeMaterial.addTexture(textures::TextureType::Roughness, AssetsManager::instance().getTextureByName("older-wood-flooring_roughness.png"));
    //
    // auto plane = std::make_shared<geometries::Cube>("plane");
    // plane->create();
    // plane->setMaterial(planeMaterial);
    // plane->setModel(AssetsManager::instance().getStaticModelByName("cube.obj"));
    // plane->setPosition({0, -0.5, 0});
    // plane->setScale({20, 0.1, 20});
    // plane->setRigidBody(physics::PhysicsController::instance().addStaticActor(plane));
    // plane->setRotation({});


    // auto voidObject = std::make_shared<Void>("void");
    // voidObject->create();
    // voidObject->setTexture(AssetsManager::instance().getTextureByName("void.png"));
    // voidObject->setModel(AssetsManager::instance().getSkinnedModelByName("void.fbx"));
    // voidObject->setScale(glm::vec3{0.003f, 0.003f, 0.003f});
    // voidObject->setRotation(180, glm::vec3{0.0, 1.0, 0.0});
    // voidObject->setPosition({5.0f, 0.0f, 0.0f});
    // voidObject->playAnimation();

    // auto cube = std::make_shared<geometries::Cube>("cube");
    // cube->create();
    // cube->setMaterial(doorMaterial);
    // cube->setModel(AssetsManager::instance().getStaticModelByName("cube.obj"));
    // cube->setPosition({1.0, 5.0, -4.0});
    // cube->setScale({1.0f, 1.0f, 1.0f});
    // cube->setRigidBody(physics::PhysicsController::instance().addDynamicActor(cube));

    CameraManager::getInstance().getActiveCamera()->setCameraMode(CameraMode::FPS);

    const std::string pathToMap = filesystem::getMapsFolderPath().string() + "/default_scene.json";

    std::ifstream file(pathToMap);
    nlohmann::json json;
    file >> json;

    for(const auto& wall : json["walls"])
    {
        Material defaultMaterial;
        defaultMaterial.addTexture(textures::TextureType::Diffuse, AssetsManager::instance().getTextureByName(wall["textureName"]));

        //TODO MAKE UNIQUE NAME
        //SCENE GAME OBJECTS SHOULD NOT HAVE SAME NAMES
        auto gameObject = std::make_shared<geometries::Cube>(wall["name"]);
        gameObject->create();
        gameObject->setMaterial(defaultMaterial);
        gameObject->setModel(AssetsManager::instance().getStaticModelByName(wall["modelName"]));
        gameObject->setPosition({wall["position"]["x"], wall["position"]["y"], wall["position"]["z"]});
        gameObject->setScale({wall["scale"]["x"], wall["scale"]["y"], wall["scale"]["z"]});
        gameObject->setRigidBody(physics::PhysicsController::instance().addStaticActor(gameObject));
        gameObject->setRotation(wall["rotation"]["angle"], {wall["rotation"]["x"], wall["rotation"]["y"], wall["rotation"]["z"]});
        m_gameObjects.push_back(gameObject);
    }

    // std::shared_ptr<Void> man = std::make_shared<Void>("man");
    // man->create();
    // man->setTexture(AssetsManager::instance().getTextureByName("man_Packed0_Diffuse.png"));
    // man->setModel(AssetsManager::instance().getSkinnedModelByName("man.fbx"));
    // man->setPosition({0.0f, 0.0f, -1.0f});
    // man->setScale({0.01f, 0.01f, 0.01f});

    m_player = std::make_shared<Player>("player");
    m_player->init({0.0f, 1.0f, 0.0f});
    // m_gameObjects.push_back(man);
    m_gameObjects.push_back(m_player);
    // m_gameObjects.push_back(voidObject);
    m_gameObjects.push_back(mannequin);
    // m_gameObjects.push_back(door);
    // m_gameObjects.push_back(cube);

    // m_gameObjects.push_back(plane);


    lighting::Light directionalLight;
    directionalLight.type = lighting::LightType::DIRECTIONAL;
    directionalLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    directionalLight.position = glm::vec3(-2.0f, 4.0f, -1.0f);
    directionalLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    directionalLight.strength = 2.0f;
    LightManager::instance().addLight(directionalLight);

    auto lightObject = std::make_shared<geometries::Cube>("light");
    lightObject->create();
    lightObject->setMaterial(doorMaterial);
    lightObject->setModel(AssetsManager::instance().getStaticModelByName("cube.obj"));
    lightObject->setPosition(directionalLight.position);
    lightObject->setScale({0.5f, 0.5f, 0.5f});
    lightObject->setRigidBody(physics::PhysicsController::instance().addStaticActor(lightObject));

    m_gameObjects.push_back(lightObject);

    auto shadersPath = filesystem::getShadersFolderPath().string();
    m_shadowShader.load(shadersPath + "/shadow_map.vert", shadersPath + "/shadow_map.frag");

    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void calculateShadows(Shader& shader, std::vector<std::shared_ptr<GameObject>>& gameObjects)
{
    glm::vec3 lightPos = LightManager::instance().getLights()[0].position;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    LightManager::instance().setLightSpaceMatrix(lightSpaceMatrix);

    shader.bind();
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    for (auto& obj : gameObjects)
        if (auto* cube = dynamic_cast<geometries::Cube*>(obj.get()))
            cube->updateShadowMap(shader);
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    auto* win = window::WindowsManager::instance().getCurrentWindow();

    glViewport(0, 0, win->getWidth(), win->getHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void DefaultScene::update(float deltaTime)
{
    calculateShadows(m_shadowShader, m_gameObjects);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    for(auto& gameObject : m_gameObjects)
        gameObject->update(deltaTime);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    debug::DebugLine debugLine;

    const auto* camera = CameraManager::getInstance().getActiveCamera();

    glm::vec3 rayOrigin = {camera->getPosition().x, camera->getPosition().y + 0.001f, camera->getPosition().z};
    glm::vec3 rayDir = glm::normalize(camera->getForward());
    float rayLength = 100.0f;
    glm::vec3 rayEnd = rayOrigin + rayDir * rayLength;

    debugLine.draw(rayOrigin, rayEnd);

    m_skybox.render();

    debug::DebugTextHolder::instance().update(deltaTime);
}

bool DefaultScene::isOver()
{
    return false;
}

DefaultScene::~DefaultScene() = default;