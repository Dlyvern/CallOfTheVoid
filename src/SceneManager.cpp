#include "SceneManager.hpp"

#include <fstream>
#include <json/json.hpp>

#include "AssetsManager.hpp"
#include "Cube.hpp"
#include "Filesystem.hpp"
#include "Void.hpp"

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

        if (auto cube = dynamic_cast<geometries::Cube*>(object.get()))
            json["model"] = cube->getModel() ? cube->getModel()->getName() : "";
        else if (auto voidObject = dynamic_cast<Void*>(object.get()))
            json["model"] = voidObject->getModel() ? voidObject->getModel()->getName() : "";

        json["position"] = {object->getPosition().x, object->getPosition().y, object->getPosition().z};
        json["scale"] = {object->getScale().x,object->getScale().y, object->getScale().z};
        json["rotation"] = {object->getRotation().x, object->getRotation().y, object->getRotation().z};
    }

    std::ofstream file(filePath);

    if (file.is_open())
    {
        file << std::setw(4) << json << std::endl;
        file.close();
    }
    else
        std::cout << "SceneManager::saveObjectsIntoFile(): Could not open file to save game objects: " << filePath << std::endl;
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
