#include "SceneManager.hpp"

#include <fstream>
#include <json/json.hpp>

#include "AssetsManager.hpp"
#include "Cube.hpp"
#include "Filesystem.hpp"

SceneManager& SceneManager::instance()
{
    static SceneManager instance;
    return instance;
}

void SceneManager::setCurrentScene(Scene *scene)
{
    m_currentScene = scene;
}

Scene* SceneManager::getCurrentScene()
{
    return m_currentScene;
}

void SceneManager::updateCurrentScene(float deltaTime)
{
    if (m_currentScene)
        m_currentScene->update(deltaTime);
}

void SceneManager::saveObjectsIntoFile(const std::vector<std::shared_ptr<GameObject>> &objects,const std::string &filePath)
{
    nlohmann::json json;

    for (const auto& object : objects)
    {
        json["name"] = object->getName();
    }

//
//     "name": "door",
// "material": "default_material.mat",
// "modelName": "jail_door.fbx",
// "position":
// {
//     "x": 1.0,
//     "y": -0.5,
//     "z": 0.0
// },
// "scale":
// {
//     "x": 1.0,
//     "y": 1.0,
//     "z": 1.0
// },
// "rotation":
// {
//     "x": 1.0,
//     "y": 0.0,
//     "z": 0.0,
//     "angle": -90
// }

}

std::vector<std::shared_ptr<GameObject>> SceneManager::loadObjectsFromFile(const std::string &filePath)
{
    std::vector<std::shared_ptr<GameObject>> objects;

    std::ifstream file(filePath);
    nlohmann::json json;
    file >> json;
    auto materialPath = filesystem::getMaterialsFolderPath().string();

    for(const auto& wall : json["walls"])
    {
        //TODO MAKE UNIQUE NAME
        auto gameObject = std::make_shared<geometries::Cube>(wall["name"]);

        if (auto material = AssetsManager::instance().getMaterialByName(wall["material"]); material)
            gameObject->setMaterial(material);

        gameObject->setModel(AssetsManager::instance().getStaticModelByName(wall["modelName"]));
        gameObject->setPosition({wall["position"]["x"], wall["position"]["y"], wall["position"]["z"]});
        gameObject->setScale({wall["scale"]["x"], wall["scale"]["y"], wall["scale"]["z"]});
        gameObject->setRigidBody(physics::PhysicsController::instance().addStaticActor(gameObject));
        gameObject->setRotation(wall["rotation"]["angle"], {wall["rotation"]["x"], wall["rotation"]["y"], wall["rotation"]["z"]});

        objects.push_back(gameObject);
    }

    return objects;
}
