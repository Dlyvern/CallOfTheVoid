#include "DefaultScene.hpp"
#include "AssetsManager.hpp"
#include "Raycasting.hpp"
#include <algorithm>
#include "../libraries/json/json.hpp"
#include <fstream>
#include "Filesystem.hpp"

DefaultScene::DefaultScene() : Scene()
{
}

void DefaultScene::create()
{
    //TODO FIX IT LATER
    // AssetsManager::instance().getTextureByName("wooden_floor.png")->bake();
    AssetsManager::instance().getTextureByName("man_Packed0_Diffuse.png")->bake();
    AssetsManager::instance().getTextureByName("wallParking_d.png")->bake();
    AssetsManager::instance().getTextureByName("concrete_d.png")->bake();

    //TODO MAKE IT MORE SAFE
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
        gameObject->setModel(AssetsManager::instance().getModelByName(wall["modelName"]));
        gameObject->setPosition({wall["position"]["x"], wall["position"]["y"], wall["position"]["z"]});
        gameObject->setScale({wall["scale"]["x"], wall["scale"]["y"], wall["scale"]["z"]});
        gameObject->setRotation(wall["rotation"]["angle"], {wall["rotation"]["x"], wall["rotation"]["y"], wall["rotation"]["z"]});
        m_gameObjects.emplace_back(gameObject);
    }

    m_man = std::make_shared<geometries::Cube>("man");
    m_man->create();
    m_man->setTexture(AssetsManager::instance().getTextureByName("man_Packed0_Diffuse.png"));
    m_man->setModel(AssetsManager::instance().getModelByName("man.fbx"));
    m_man->setPosition({0.0f, -0.25f, -1.0f});
    m_man->setScale({0.01f, 0.01f, 0.01f});

    m_player = std::make_shared<Player>("player");
    m_player->setPosition({0.0f, 1.0f, 0.0f});

    m_gameObjects.emplace_back(m_man);
    m_gameObjects.emplace_back(m_player);
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


    // for(const auto& firstObject : m_gameObjects)
    // {
    //     for(const auto& secondObject : m_gameObjects)
    //     {
    //         if(firstObject != secondObject)
    //         {
    //             if(physics::collision::doAABBIntersectABBB(firstObject->getBoundingBox(), secondObject->getBoundingBox()))
    //             {
    //                 float overlapX = std::max(0.0f, std::min(firstObject->getBoundingBox().max.x, secondObject->getBoundingBox().max.x) - std::max(firstObject->getBoundingBox().min.x, secondObject->getBoundingBox().min.x));
    //                 float overlapY = std::max(0.0f, std::min(firstObject->getBoundingBox().max.y, secondObject->getBoundingBox().max.y) - std::max(firstObject->getBoundingBox().min.y, secondObject->getBoundingBox().min.y));
    //                 float overlapZ = std::max(0.0f, std::min(firstObject->getBoundingBox().max.z, secondObject->getBoundingBox().max.z) - std::max(firstObject->getBoundingBox().min.z, secondObject->getBoundingBox().min.z));

    //                 // Determine the axis with the smallest overlap
    //                 float smallestOverlap = std::min({overlapX, overlapY, overlapZ});

    //                 glm::vec3 adjustment(0.0f);

    //                 if (smallestOverlap == overlapX)
    //                 {
    //                     if (firstObject->getBoundingBox().min.x < secondObject->getBoundingBox().min.x)
    //                         adjustment.x = -smallestOverlap;
    //                     else
    //                         adjustment.x = smallestOverlap;
    //                 } 
    //                 else if (smallestOverlap == overlapY)
    //                 {
    //                     if (firstObject->getBoundingBox().min.y < secondObject->getBoundingBox().min.y)
    //                         adjustment.y = -smallestOverlap;
    //                     else
    //                         adjustment.y = smallestOverlap;
    //                 } 
    //                 else 
    //                 {
    //                     if (firstObject->getBoundingBox().min.z < secondObject->getBoundingBox().min.z)
    //                         adjustment.z = -smallestOverlap;
    //                     else
    //                         adjustment.z = smallestOverlap;
    //                 }

    //                 firstObject->setPosition(firstObject->getPosition() + adjustment);
    //             }
    //         }
    //     }
    // }

    for(const auto& object : m_gameObjects)
    {
        if(object == m_player)
            continue;

        if(!physics::collision::doAABBIntersectABBB(object->getBoundingBox(), m_player->getBoundingBox()))
            continue;

        float overlapX = std::max(0.0f, std::min(m_player->getBoundingBox().max.x, object->getBoundingBox().max.x) - std::max(m_player->getBoundingBox().min.x, object->getBoundingBox().min.x));
        float overlapY = std::max(0.0f, std::min(m_player->getBoundingBox().max.y, object->getBoundingBox().max.y) - std::max(m_player->getBoundingBox().min.y, object->getBoundingBox().min.y));
        float overlapZ = std::max(0.0f, std::min(m_player->getBoundingBox().max.z, object->getBoundingBox().max.z) - std::max(m_player->getBoundingBox().min.z, object->getBoundingBox().min.z));

        // Determine the axis with the smallest overlap
        float smallestOverlap = std::min({overlapX, overlapY, overlapZ});

        glm::vec3 adjustment(0.0f);

        if (smallestOverlap == overlapX) 
        {
            if (m_player->getBoundingBox().min.x < object->getBoundingBox().min.x)
                adjustment.x = -smallestOverlap;
            else
                adjustment.x = smallestOverlap;
        } 
        else if (smallestOverlap == overlapY)
        {
            if (m_player->getBoundingBox().min.y < object->getBoundingBox().min.y)
                adjustment.y = -smallestOverlap;
            else
                adjustment.y = smallestOverlap;
        } 
        else 
        {
            if (m_player->getBoundingBox().min.z < object->getBoundingBox().min.z)
                adjustment.z = -smallestOverlap;
            else
                adjustment.z = smallestOverlap;
        }

        // Adjust the player position
        m_player->setPosition(m_player->getPosition() + adjustment);
    }

    // m_grid.checkCollisions(m_gameObjects);
}

bool DefaultScene::isOver()
{
    return false;
}

DefaultScene::~DefaultScene() = default;