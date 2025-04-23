#include "DebugEditor.hpp"

#include <glad.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "AssetsManager.hpp"
#include "CameraManager.hpp"
#include "Cube.hpp"
#include "DebugLine.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "Raycasting.hpp"
#include "SceneManager.hpp"
#include "Void.hpp"
#include "WindowsManager.hpp"

DebugEditor& DebugEditor::instance()
{
    static DebugEditor instance;
    return instance;
}

void DebugEditor::update()
{
    auto* window = window::WindowsManager::instance().getCurrentWindow();

    if (input::Keyboard.isKeyReleased(input::KeyCode::K))
    {
        m_isDebugMode = !m_isDebugMode;

        if (m_isDebugMode)
            glfwSetInputMode(window->getOpenGLWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window->getOpenGLWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (!m_isDebugMode)
        return;

    if (input::Mouse.isLeftButtonPressed())
    {
        glm::vec2 mouseNDC;

        auto* camera = CameraManager::getInstance().getActiveCamera();
        double xpos, ypos;
        glfwGetCursorPos(window->getOpenGLWindow(), &xpos, &ypos);

        float x = (2.0f * xpos) / static_cast<float>(window->getWidth()) - 1.0f;
        float y = 1.0f - (2.0f * ypos) / static_cast<float>(window->getHeight());

        mouseNDC = glm::vec2(x, y);

        float aspectRatio = static_cast<float>(window->getWidth()) / static_cast<float>(window->getHeight());

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
        glm::mat4 view = camera->getViewMatrix();

        glm::vec4 rayClip(mouseNDC.x, mouseNDC.y, -1.0f, 1.0f);
        glm::vec4 rayEye = glm::inverse(projection) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));

        glm::vec3 origin = glm::vec3(glm::inverse(view)[3]);

        physics::raycasting::Ray ray{};
        physics::raycasting::RaycastingResult result;

        ray.maxDistance = 1000.0f;
        ray.direction = rayWorld;
        ray.origin = origin;

        if (physics::raycasting::shoot(ray, result))
        {
            const auto* actor = result.hit.block.actor;

            auto* gameObject = static_cast<GameObject*>(actor->userData);

            m_selectedGameObject = gameObject;
        }
    }

    showDebugInfo();
}

DebugEditor::~DebugEditor() = default;

bool DebugEditor::getDebugMode()
{
    return m_isDebugMode;
}

void DebugEditor::showDebugInfo()
{
    showObjectInfo(m_selectedGameObject);

    ImGui::Begin("Objects");

    auto allModels = AssetsManager::instance().getAllLoadedModelsNames();

    for (auto& modelName : allModels)
    {
        if (ImGui::Button(modelName.c_str()))
        {
            //TODO: make it not like this....
            if (auto skinnedModel = AssetsManager::instance().getSkinnedModelByName(modelName); skinnedModel)
            {
                auto skinnedGameObject = std::make_shared<Void>("new_void");
                skinnedGameObject->setModel(skinnedModel);
                skinnedGameObject->setRigidBody(physics::PhysicsController::instance().addStaticActor(skinnedGameObject));
                skinnedGameObject->setPosition({0.0f, 0.0f, 0.0f});
                SceneManager::instance().getCurrentScene()->addGameObject(skinnedGameObject);
                m_selectedGameObject = skinnedGameObject.get();
            }
            else if (auto staticModel = AssetsManager::instance().getStaticModelByName(modelName); staticModel)
            {
                auto staticGameObject = std::make_shared<geometries::Cube>("new_cube");
                staticGameObject->setRigidBody(physics::PhysicsController::instance().addStaticActor(staticGameObject));
                staticGameObject->setModel(staticModel);
                SceneManager::instance().getCurrentScene()->addGameObject(staticGameObject);
                m_selectedGameObject = staticGameObject.get();
            }
        }

    }

    ImGui::End();
}

void DebugEditor::showObjectInfo(GameObject* gameObject)
{
    if (!gameObject)
        return;

    ImGui::Begin("Engine Debug");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("GameObject name: %s", m_selectedGameObject->getName().c_str());

    if (ImGui::BeginTabBar("Tabs"))
    {
        if (ImGui::BeginTabItem("Transform"))
        {
            glm::vec3 position = m_selectedGameObject->getPosition();
            ImGui::Text("Position");
            bool positionEdited = ImGui::InputFloat3("Position", &position[0]);

            if (positionEdited && ImGui::IsItemDeactivatedAfterEdit())
                m_selectedGameObject->setPosition(position);

            glm::vec3 rotation = m_selectedGameObject->getRotation();
            ImGui::Text("Rotation");
            bool rotationEdited = ImGui::InputFloat3("Rotation", &rotation[0]);

            if (rotationEdited && ImGui::IsItemDeactivatedAfterEdit())
                m_selectedGameObject->setRotation(90, rotation);

            glm::vec3 scale = m_selectedGameObject->getScale();
            ImGui::Text("Scale");
            bool sizeEdited = ImGui::InputFloat3("Scale", &scale[0]);

            if (sizeEdited && ImGui::IsItemDeactivatedAfterEdit())
                m_selectedGameObject->setScale(scale);

            if (auto skeleton = dynamic_cast<Void*>(m_selectedGameObject); skeleton)
            {
                ImGui::Text("Material %s", skeleton->getMaterial().getName().c_str());
                ImGui::SameLine();

                std::vector<std::string> loadedMaterials = AssetsManager::instance().getAllLoadedMaterialsNames();

                std::vector<const char*> materialNamesCStr;

                for (const auto& mat : loadedMaterials)
                    materialNamesCStr.push_back(mat.c_str());

                auto it = std::find_if(materialNamesCStr.begin(), materialNamesCStr.end(),
                [&skeleton](const char* materialName){return skeleton->getMaterial().getName().c_str() == materialName;});

                static int selectedMaterial = std::distance(materialNamesCStr.begin(), it);

                if (ImGui::Combo("##Material combo", &selectedMaterial, materialNamesCStr.data(), static_cast<int>(materialNamesCStr.size())))
                {
                    if (auto mat = AssetsManager::instance().getMaterialByName(materialNamesCStr[selectedMaterial]); mat)
                        skeleton->setMaterial(*mat);
                }

                ImGui::Text("Model %s", skeleton->getModel()->getName().c_str());
                ImGui::SameLine();

                auto models = AssetsManager::instance().getAllLoadedModelsNames();

                std::vector<const char*> modelNames;

                for (const auto& modelName : models)
                    modelNames.push_back(modelName.c_str());

                auto i = std::find_if(modelNames.begin(), modelNames.end(),
                [&skeleton](const std::string& modelName){return skeleton->getModel()->getName() == modelName;});

                static int selectedModel = std::distance(modelNames.begin(), i);

                if (ImGui::Combo("##Model combo", &selectedModel, modelNames.data(), static_cast<int>(modelNames.size())))
                {
                    if (auto m = AssetsManager::instance().getSkinnedModelByName(modelNames[selectedModel]); m)
                        skeleton->setModel(m);
                }
            }
            else if (auto cube = dynamic_cast<geometries::Cube*>(m_selectedGameObject); cube)
            {
                ImGui::Text("Material %s", cube->getMaterial().getName().c_str());
                ImGui::SameLine();

                std::vector<std::string> loadedMaterials = AssetsManager::instance().getAllLoadedMaterialsNames();

                std::vector<const char*> materialNamesCStr;

                for (const auto& mat : loadedMaterials)
                    materialNamesCStr.push_back(mat.c_str());

                auto it = std::find_if(materialNamesCStr.begin(), materialNamesCStr.end(),
                [&cube](const char* materialName){return cube->getMaterial().getName().c_str() == materialName;});

                static int selectedMaterial = std::distance(materialNamesCStr.begin(), it);

                if (ImGui::Combo("##Material combo", &selectedMaterial, materialNamesCStr.data(), static_cast<int>(materialNamesCStr.size())))
                    cube->setMaterial(AssetsManager::instance().getMaterialByName(materialNamesCStr[selectedMaterial]));

                ImGui::Text("Model %s", cube->getModel()->getName().c_str());
                ImGui::SameLine();

                auto models = AssetsManager::instance().getAllLoadedModelsNames();

                std::vector<const char*> modelNames;

                for (const auto& modelName : models)
                    modelNames.push_back(modelName.c_str());

                auto i = std::find_if(modelNames.begin(), modelNames.end(),
                [&cube](const std::string& modelName){return cube->getModel()->getName() == modelName;});

                static int selectedModel = std::distance(modelNames.begin(), i);

                if (ImGui::Combo("##Model combo", &selectedModel, modelNames.data(), static_cast<int>(modelNames.size())))
                {
                    if (auto m = AssetsManager::instance().getStaticModelByName(modelNames[selectedModel]); m)
                        cube->setModel(m);
                }

            }

            ImGui::EndTabItem();
        }

        if (auto skeleton = dynamic_cast<Void*>(m_selectedGameObject); skeleton && ImGui::BeginTabItem("Bones"))
        {
            displayBonesHierarchy(skeleton->getModel() ? &skeleton->getModel()->getSkeleton() : nullptr);
            ImGui::EndTabItem();
        }

        if (auto skeleton = dynamic_cast<Void*>(m_selectedGameObject); skeleton && ImGui::BeginTabItem("Animation"))
        {
            const auto& animations = skeleton->getModel()->getAnimations();

            for (const auto& anim : animations)
                ImGui::Text("Animation %s", anim.name.c_str());

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void DebugEditor::displayBonesHierarchy(Skeleton* skeleton, common::BoneInfo* parent)
{
    if (!skeleton)
        return;

    common::BoneInfo* bone = parent ? parent : skeleton->getParent();

    if (ImGui::TreeNode(bone->name.c_str()))
    {
        for (const auto& childBone : bone->children)
            displayBonesHierarchy(skeleton, skeleton->getBone(childBone));

        ImGui::TreePop();
    }
}

DebugEditor::DebugEditor() = default;
