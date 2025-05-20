#include "SceneManager.hpp"

#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <json/json.hpp>

#include "AnimatorComponent.hpp"
#include "AssetsManager.hpp"
#include "DoorScript.hpp"
#include "Filesystem.hpp"
#include "Light.hpp"
#include "LightComponent.hpp"
#include "LightManager.hpp"
#include "RigidbodyComponent.hpp"
#include "ScriptsRegister.hpp"
#include "SkeletalMeshComponent.hpp"
#include "StaticMeshComponent.hpp"

class LightComponent;

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

void SceneManager::addScene(Scene *scene)
{
    scene->setOnSceneEndCallback(std::bind(&SceneManager::onSceneIsOver, this, std::placeholders::_1));
    m_scenes.push_back(scene);
}

void SceneManager::onSceneIsOver(Scene* scene)
{
    m_scenes.erase(std::find(m_scenes.begin(), m_scenes.end(), scene));
    delete scene;

    if (!m_scenes.empty())
    {
        Scene* newScene = m_scenes.back();
        newScene->create();

        setCurrentScene(newScene);
    }
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
        if (object->getName() == "player")
            continue;

        nlohmann::json objectJson;

        objectJson["name"] = object->getName();

        if (object->hasComponent<LightComponent>())
        {
            nlohmann::json lightJson;

            auto* light = object->getComponent<LightComponent>()->getLight();

            lightJson["type"] = "LightComponent";
            lightJson["lightType"] = static_cast<int>(light->type);
            lightJson["direction"] = {light->direction.x, light->direction.y, light->direction.z};
            lightJson["color"] = {light->color.r, light->color.g, light->color.b};
            lightJson["strength"] = light->strength;
            lightJson["position"] = {light->position.x, light->position.y, light->position.z};
            lightJson["radius"] = light->radius;

            objectJson["components"].push_back(lightJson);
        }
        if (object->hasComponent<AnimatorComponent>())
        {
            nlohmann::json animatorJson;

            animatorJson["type"] = "AnimatorComponent";

            objectJson["components"].push_back(animatorJson);
        }
        if (object->hasComponent<ScriptComponent>())
        {
            nlohmann::json scriptJson;

            scriptJson["type"] = "ScriptComponent";

            for (const auto& script : object->getComponent<ScriptComponent>()->getScripts())
            {
                scriptJson["scripts"].push_back({
                    {"script", script->getScriptName()}
                });
            }

            objectJson["components"].push_back(scriptJson);
        }

        if (object->hasComponent<StaticMeshComponent>())
        {
            auto model = object->getComponent<StaticMeshComponent>()->getModel();
            objectJson["model"] = model ? model->getName() : "";

            nlohmann::json materialJson;

            for (int index = 0; index < model->getMeshesSize(); index++)
            {
                auto mesh = model->getMesh(index);

                Material* material;

                if (object->overrideMaterials.contains(index))
                    material = object->overrideMaterials[index];
                else
                    material = mesh->getMaterial();

                if (material)
                {
                    materialJson[std::to_string(index)] = material->getName();
                }
            }

            objectJson["materials"] = materialJson;
        }
        else if (object->hasComponent<SkeletalMeshComponent>())
        {
            auto model = object->getComponent<SkeletalMeshComponent>()->getModel();
            objectJson["skinnedModel"] = model ? model->getName() : "";

            nlohmann::json materialJson;

            for (int index = 0; index < model->getMeshesSize(); index++)
            {
                auto mesh = model->getMesh(index);

                Material* material;

                if (object->overrideMaterials.contains(index))
                    material = object->overrideMaterials[index];
                else
                    material = mesh->getMaterial();

                if (material)
                {
                    materialJson[std::to_string(index)] = material->getName();
                }
            }

            objectJson["materials"] = materialJson;

        }

        objectJson["position"] = {object->getPosition().x, object->getPosition().y, object->getPosition().z};
        objectJson["scale"] = {object->getScale().x, object->getScale().y, object->getScale().z};
        objectJson["rotation"] = {object->getRotation().x, object->getRotation().y, object->getRotation().z};

        json["walls"].push_back(objectJson);
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

std::vector<std::shared_ptr<GameObject>> SceneManager::loadObjectsFromFile(const std::string& filePath)
{
    std::vector<std::shared_ptr<GameObject>> objects;

    std::ifstream file(filePath);
    nlohmann::json json;
    file >> json;

    for (const auto& objectJson : json["walls"])
    {
        std::shared_ptr<GameObject> gameObject;
        const std::string& name = objectJson["name"];

        const bool isSkinned = objectJson.contains("skinnedModel");
        const std::string modelName = objectJson[isSkinned ? "skinnedModel" : "model"];
        const std::string modelPath = filesystem::getModelsFolderPath().string() + '/' + modelName;

        common::Model* model{nullptr};

        if (isSkinned)
        {
            auto voidObject = std::make_shared<GameObject>(name);
            auto m = AssetsManager::instance().getSkinnedModelByName(modelName);
            voidObject->addComponent<SkeletalMeshComponent>(m);
            model = m;
            gameObject = voidObject;
        }
        else
        {
            auto cube = std::make_shared<GameObject>(name);
            auto m = AssetsManager::instance().getStaticModelByName(modelName);
            cube->addComponent<StaticMeshComponent>(m);
            model = m;
            gameObject = cube;
        }

        auto& overrideMaterials = gameObject->overrideMaterials;

        if (objectJson.contains("materials"))
        {
            const auto& materials = objectJson["materials"];
            for (int i = 0; i < model->getMeshesSize(); ++i)
            {
                const std::string indexStr = std::to_string(i);
                if (materials.contains(indexStr))
                {
                    const std::string materialName = materials[indexStr];
                    if (!materialName.empty())
                    {
                        auto material = AssetsManager::instance().getMaterialByName(materialName);
                        overrideMaterials[i] = material;
                    }
                }
            }
        }
        else
        {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                std::cerr << "Assimp load error: " << importer.GetErrorString() << std::endl;
                throw std::runtime_error("Could not load model: " + modelPath);
            }

            auto loadMaterialsRecursively = [&overrideMaterials](aiNode* node, const aiScene* scene, int meshIndex, auto&& self) -> void
            {
                for (unsigned int i = 0; i < node->mNumMeshes; ++i)
                {
                    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                    unsigned int materialIndex = mesh->mMaterialIndex;
                    aiMaterial* aiMat = scene->mMaterials[materialIndex];

                    if (aiMat)
                    {
                        if (auto mat = AssetsManager::instance().loadMaterialFromModel(aiMat))
                        {
                            overrideMaterials[meshIndex] = mat;
                            std::cout << "Loaded material: " << mat->getName() << '\n';
                        }
                        else std::cout << "Failed to load material.\n";
                    }
                }

                for (unsigned int i = 0; i < node->mNumChildren; ++i)
                    self(node->mChildren[i], scene, meshIndex, self);
            };

            for (int i = 0; i < model->getMeshesSize(); ++i)
                loadMaterialsRecursively(scene->mRootNode, scene, i, loadMaterialsRecursively);
        }

        if (objectJson.contains("position"))
        {
            const auto& pos = objectJson["position"];
            gameObject->setPosition({ pos[0], pos[1], pos[2] });
        }

        if (objectJson.contains("scale"))
        {
            const auto& scale = objectJson["scale"];
            gameObject->setScale({ scale[0], scale[1], scale[2] });
        }

        if (objectJson.contains("rotation"))
        {
            const auto& rot = objectJson["rotation"];
            gameObject->setRotation({ rot[0], rot[1], rot[2] });
        }

        gameObject->addComponent<RigidbodyComponent>(gameObject);

        if (isSkinned)
            physics::PhysicsController::instance().resizeCollider({1.0f, 2.0f, 1.0f}, gameObject);

        if (objectJson.contains("components"))
        {
            for (const auto& componentJson : objectJson["components"])
            {
                if (componentJson["type"] == "LightComponent")
                {
                    lighting::Light light;
                    light.type = static_cast<lighting::LightType>(componentJson["lightType"]);
                    const auto& direction = componentJson["direction"];
                    light.direction = glm::vec3(direction[0], direction[1], direction[2]);
                    const auto& position = componentJson["position"];
                    light.position = glm::vec3(position[0], position[1], position[2]);
                    const auto& color = componentJson["color"];
                    light.color = glm::vec3(color[0], color[1], color[2]);
                    light.strength = componentJson["strength"];
                    light.radius = componentJson["radius"];
                    gameObject->addComponent<LightComponent>(light);

                    LightManager::instance().addLight(gameObject->getComponent<LightComponent>()->getLight());
                }
                else if (componentJson["type"] == "AnimatorComponent")
                {
                    gameObject->addComponent<AnimatorComponent>();
                }
                else if (componentJson["type"] == "ScriptComponent")
                {
                    auto* scriptComponent = gameObject->addComponent<ScriptComponent>();

                    if (componentJson.contains("scripts")) {
                        for (auto& scriptEntry : componentJson["scripts"]) {
                            const std::string scriptName = scriptEntry["script"];
                            auto script = ScriptsRegister::instance().createScript(scriptName);
                            scriptComponent->addScript(std::move(script));
                        }
                    }

                    // gameObject->getComponent<ScriptComponent>()->addScript(ScriptsRegister::instance().createScript(componentJson["script"]));
                }
            }
        }

        objects.push_back(gameObject);
    }

    return objects;
}