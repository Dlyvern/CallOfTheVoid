#include "SceneManager.hpp"

#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
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
        // if (auto voidObject = dynamic_cast<Void*>(object.get()))
        //     continue;

        if (object->getName() == "player")
            continue;

        nlohmann::json objectJson;

        objectJson["name"] = object->getName();

        if (auto cube = dynamic_cast<geometries::Cube*>(object.get()))
        {
            auto model = cube->getModel();
            objectJson["model"] = model ? model->getName() : "";

            nlohmann::json materialJson;

            for (int index = 0; index < model->getMeshesSize(); index++)
            {
                auto mesh = model->getMesh(index);

                Material* material;

                if (cube->overrideMaterials.contains(index))
                    material = cube->overrideMaterials[index];
                else
                    material = mesh->getMaterial();

                if (material)
                {
                    materialJson[std::to_string(index)] = material->getName();
                }
            }

            objectJson["materials"] = materialJson;
        }
        else if (auto voidObject = dynamic_cast<Void*>(object.get()))
        {
            auto model = voidObject->getModel();
            objectJson["skinnedModel"] = model ? model->getName() : "";

            nlohmann::json materialJson;

            for (int index = 0; index < model->getMeshesSize(); index++)
            {
                auto mesh = model->getMesh(index);

                Material* material;

                if (voidObject->overrideMaterials.contains(index))
                    material = voidObject->overrideMaterials[index];
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

        auto* model = isSkinned ?
            static_cast<common::Model*>(AssetsManager::instance().getSkinnedModelByName(modelName)) :
            static_cast<common::Model*>(AssetsManager::instance().getStaticModelByName(modelName));

        if (isSkinned)
        {
            auto voidObject = std::make_shared<Void>(name);
            voidObject->setModel(dynamic_cast<SkinnedModel*>(model));
            gameObject = voidObject;
        }
        else
        {
            auto cube = std::make_shared<geometries::Cube>(name);
            cube->setModel(dynamic_cast<StaticModel*>(model));
            gameObject = cube;
        }

        auto& overrideMaterials = isSkinned ?
            dynamic_cast<Void*>(gameObject.get())->overrideMaterials :
            dynamic_cast<geometries::Cube*>(gameObject.get())->overrideMaterials;

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
            gameObject->setRotation(0, { rot[0], rot[1], rot[2] });
        }

        if (isSkinned)
            dynamic_cast<Void*>(gameObject.get())->setRigidBody(physics::PhysicsController::instance().addStaticActor(gameObject, true));
        else
            dynamic_cast<geometries::Cube*>(gameObject.get())->setRigidBody(physics::PhysicsController::instance().addStaticActor(gameObject));

        objects.push_back(gameObject);
    }

    return objects;
}