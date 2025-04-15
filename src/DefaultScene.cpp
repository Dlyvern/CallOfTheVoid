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

    auto door = std::make_shared<geometries::Cube>("door");
    door->create();
    door->setTexture(AssetsManager::instance().getTextureByName("concrete_d.png"));
    door->setModel(AssetsManager::instance().getStaticModelByName("Jail_Door.fbx"));
    door->setRotation(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    door->setPosition({1.0, 0.0, 0.0});
    door->setScale({1.0f, 1.0f, 1.0f});
    door->setRigidBody(physics::PhysicsController::instance().addStaticActor(door));

    auto mannequin = std::make_shared<Void>("mannequin");
    mannequin->create();
    mannequin->setTexture(AssetsManager::instance().getTextureByName("void.png"));
    mannequin->setModel(AssetsManager::instance().getSkinnedModelByName("mannequin.fbx"));
    mannequin->setPosition(glm::vec3(-5.0f, 0.0f, 0.0f));
    mannequin->setScale(glm::vec3(0.01, 0.01, 0.01));
    mannequin->playAnimation();

    auto voidObject = std::make_shared<Void>("void");
    voidObject->create();
    voidObject->setTexture(AssetsManager::instance().getTextureByName("void.png"));
    voidObject->setModel(AssetsManager::instance().getSkinnedModelByName("void.fbx"));
    voidObject->setScale(glm::vec3{0.003f, 0.003f, 0.003f});
    voidObject->setRotation(180, glm::vec3{0.0, 1.0, 0.0});
    voidObject->setPosition({5.0f, 0.0f, 0.0f});
    voidObject->playAnimation();

    auto cube = std::make_shared<geometries::Cube>("cube");
    cube->create();
    cube->setTexture(AssetsManager::instance().getTextureByName("concrete_d.png"));
    cube->setModel(AssetsManager::instance().getStaticModelByName("cube.obj"));
    cube->setPosition({1.0, 5.0, -4.0});
    cube->setScale({1.0f, 1.0f, 1.0f});
    cube->setRigidBody(physics::PhysicsController::instance().addDynamicActor(cube));

    CameraManager::getInstance().getActiveCamera()->setCameraMode(CameraMode::FPS);

    const std::string pathToMap = filesystem::getMapsFolderPath().string() + "/default_scene.json";

    std::ifstream file(pathToMap);
    nlohmann::json json;
    file >> json;

    for(const auto& wall : json["walls"])
    {
        //TODO MAKE UNIQUE NAME
        //SCENE GAME OBJECTS SHOULD NOT HAVE SAME NAMES
        auto gameObject = std::make_shared<geometries::Cube>(wall["name"]);
        gameObject->create();
        gameObject->setTexture(AssetsManager::instance().getTextureByName(wall["textureName"]));
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
    // man->setPosition({0.0f, -0.25f, -1.0f});
    // man->setScale({0.01f, 0.01f, 0.01f});

    m_player = std::make_shared<Player>("player");
    m_player->init({0.0f, 1.0f, 0.0f});
    // m_gameObjects.push_back(man);
    m_gameObjects.push_back(m_player);
    // m_gameObjects.push_back(voidObject);
    m_gameObjects.push_back(cube);
    m_gameObjects.push_back(mannequin);
    m_gameObjects.push_back(door);


    lighting::Light pointLight;
    pointLight.position = glm::vec3(0.0f, 5.0f, 2.0f);
    // pointLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    // pointLight.strength = 0.1;
    LightManager::instance().addLight(pointLight);
}

void DefaultScene::update(float deltaTime)
{
    for(const auto& gameObject : m_gameObjects)
        gameObject->update(deltaTime);

    debug::DebugLine debugLine;

    const auto* camera = CameraManager::getInstance().getActiveCamera();

    glm::vec3 rayOrigin = camera->getPosition();
    glm::vec3 rayDir = glm::normalize(camera->getForward());
    float rayLength = 100.0f;
    glm::vec3 rayEnd = rayOrigin + rayDir * rayLength;

    // glm::vec3 from(camera->getPosition().x, camera->getPosition().y, 0.0f);

    glm::vec3 from(camera->getPosition().x, camera->getPosition().y + 0.001f, camera->getPosition().z);
    glm::vec3 to = from + glm::normalize(camera->getForward()) * 100.0f;

    debugLine.draw(from, to);

    m_skybox.render();
}

bool DefaultScene::isOver()
{
    return false;
}

DefaultScene::~DefaultScene() = default;