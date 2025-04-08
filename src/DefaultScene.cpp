#include "DefaultScene.hpp"
#include "AssetsManager.hpp"
#include "Raycasting.hpp"
#include <algorithm>
#include "../libraries/json/json.hpp"
#include <fstream>

#include "CameraManager.hpp"
#include "Filesystem.hpp"
#include "Light.hpp"
#include "LightManager.hpp"
#include "Physics.hpp"
std::shared_ptr<Void> voidObject{nullptr};
std::shared_ptr<geometries::Cube> cube{nullptr};

DefaultScene::DefaultScene() : Scene()
{
}

void DefaultScene::create()
{
    // //TODO FIX IT LATER
    AssetsManager::instance().getTextureByName("wooden_floor.png")->bake();
    AssetsManager::instance().getTextureByName("man_Packed0_Diffuse.png")->bake();
    AssetsManager::instance().getTextureByName("wallParking_d.png")->bake();
    AssetsManager::instance().getTextureByName("concrete_d.png")->bake();

    voidObject = std::make_shared<Void>("Void");
    voidObject->create();

    voidObject->setTexture(AssetsManager::instance().getTextureByName("void.png"));
    voidObject->setModel(AssetsManager::instance().getSkinnedModelByName("void.fbx"));
    voidObject->setScale(glm::vec3{0.003f, 0.003f, 0.003f});
    voidObject->setRotation(180, glm::vec3{0.0, 1.0, 0.0});
    voidObject->setPosition({5.0f, 0.0f, 0.0f});
    // voidObject->playAnimation();

    cube = std::make_shared<geometries::Cube>("Cube");
    cube->create();
    cube->setTexture(AssetsManager::instance().getTextureByName("concrete_d.png"));
    cube->setModel(AssetsManager::instance().getStaticModelByName("cube.obj"));
    cube->setPosition({1.0, 5.0, -4.0});
    cube->setScale({1.0f, 1.0f, 1.0f});
    cube->setRigidBody(physics::PhysicsController::instance().addDynamicActor(cube));

    CameraManager::getInstance().getActiveCamera()->setCameraMode(CameraMode::FPS);

    auto pathToMap = filesystem::getMapsFolderPath().string() + "/default_scene.json";

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
        gameObject->setRotation(wall["rotation"]["angle"], {wall["rotation"]["x"], wall["rotation"]["y"], wall["rotation"]["z"]});
        gameObject->setRigidBody(physics::PhysicsController::instance().addStaticActor(gameObject));
        m_gameObjects.emplace_back(gameObject);
    }

    // m_man = std::make_shared<Void>("man");
    // m_man->create();
    // m_man->setTexture(AssetsManager::instance().getTextureByName("man_Packed0_Diffuse.png"));
    // m_man->setModel(AssetsManager::instance().getSkinnedModelByName("man.fbx"));
    // m_man->setPosition({0.0f, -0.25f, -1.0f});
    // m_man->setScale({0.01f, 0.01f, 0.01f});

    m_player = std::make_shared<Player>("player");
    m_player->setPosition({0.0f, 1.0f, 0.0f});
    m_player->init({0.0f, 1.0f, 0.0f});
    // m_gameObjects.emplace_back(m_man);
    m_gameObjects.emplace_back(m_player);
    m_gameObjects.push_back(voidObject);
    m_gameObjects.push_back(cube);


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

    // physics::raycasting::Ray ray{};
    // ray.origin = m_player->getCamera()->getPosition();
    // ray.direction = -m_player->getCamera()->getUp();
    // ray.objects = &m_gameObjects;
    //
    // physics::raycasting::RaycastingResult result;
    //
    // if(physics::raycasting::shoot(ray, result))
    // {
    //     for (const auto& gameObject : result.objects)
    //         if (gameObject->getName() == "man")
    //             std::cout << "Yes" << std::endl;
    // }
}

bool DefaultScene::isOver()
{
    return false;
}

DefaultScene::~DefaultScene() = default;