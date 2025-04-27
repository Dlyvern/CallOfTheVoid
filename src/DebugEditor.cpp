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

namespace {
    inline std::string fromTypeToString(const GLitch::Texture::TextureType& type)
    {
        switch (type)
        {
            case GLitch::Texture::TextureType::Diffuse:
                return "Diffuse";
            case GLitch::Texture::TextureType::Specular:
                return "Specular";
            case GLitch::Texture::TextureType::Normal:
                return "Normal";
            case GLitch::Texture::TextureType::Metallic:
                return "Metallic";
            case GLitch::Texture::TextureType::Roughness:
                return "Roughness";
            case GLitch::Texture::TextureType::AO:
                return "AO";
            case GLitch::Texture::TextureType::Emissive:
                return "Emissive";
            case GLitch::Texture::TextureType::Height:
                return "Height";
            case GLitch::Texture::TextureType::Glossiness:
                return "Glossiness";
            case GLitch::Texture::TextureType::Opacity:
                return "Opacity";
            default:
                return "Undefined";
        }
    }

    inline GLitch::Texture::TextureType fromStringToTextureType(const std::string& type)
    {
        if (type == "Diffuse")
            return GLitch::Texture::TextureType::Diffuse;
        if (type == "Specular")
            return GLitch::Texture::TextureType::Specular;
        if (type == "Normal")
            return GLitch::Texture::TextureType::Normal;
        if (type == "Metallic")
            return GLitch::Texture::TextureType::Metallic;
        if (type == "Roughness")
            return GLitch::Texture::TextureType::Roughness;
        if (type == "AO")
            return GLitch::Texture::TextureType::AO;
        if (type == "Emissive")
            return GLitch::Texture::TextureType::Emissive;
        if (type == "Height")
            return GLitch::Texture::TextureType::Height;
        if (type == "Glossiness")
            return GLitch::Texture::TextureType::Glossiness;
        if (type == "Opacity")
            return GLitch::Texture::TextureType::Opacity;
        return GLitch::Texture::TextureType::Undefined;
    }
}




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

    if (input::Keyboard.isKeyReleased(input::KeyCode::DELETE))
    {
        if (m_selectedGameObject)
        {
            if (SceneManager::instance().getCurrentScene()->deleteGameObject(m_selectedGameObject))
                m_selectedGameObject = nullptr;
        }
    }
    if (input::Keyboard.isKeyReleased(input::KeyCode::O)) {
        SceneManager::saveObjectsIntoFile(SceneManager::instance().getCurrentScene()->getGameObjects(), filesystem::getMapsFolderPath().string() + "/test_scene.json");
    }

    if (!m_isDebugMode)
        return;


    if (input::Mouse.isLeftButtonPressed())
    {
        ImGuiIO& io = ImGui::GetIO();

        if (!io.WantCaptureMouse)
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
    }

    showDebugInfo();
}

DebugEditor::~DebugEditor() = default;

bool DebugEditor::getDebugMode()
{
    return m_isDebugMode;
}

void DebugEditor::showAllObjectsInTheScene()
{
    auto objects = SceneManager::instance().getCurrentScene()->getGameObjects();

    ImGui::Begin("Scene hierarchy");

    for (const auto& object : objects)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (object.get() == m_selectedGameObject)
            flags |= ImGuiTreeNodeFlags_Selected;

        bool isNodeOpened = ImGui::TreeNodeEx((void*)(intptr_t)object.get(), flags, "%s", object->getName().c_str());

        if (ImGui::IsItemClicked())
            m_selectedGameObject = object.get();

        if (isNodeOpened)
        {
            // for (auto& child : object->children)
            //     ShowGameObjectNode(child);
            ImGui::TreePop();
        }
    }

    ImGui::End();

    ImGui::Begin("Benchmark");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::End();
}

void BeginDockSpace()
{
    ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                    ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpaceWindow", nullptr, windowFlags);

    ImGui::PopStyleVar(3);

    ImGuiID dockSpaceId = ImGui::GetID("MyDockspace");
    ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), dockSpaceFlags);

    static bool dock_built = false;

    if (!dock_built)
    {
        dock_built = true;

        ImGui::DockBuilderRemoveNode(dockSpaceId);
        ImGui::DockBuilderAddNode(dockSpaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockSpaceId, viewport->WorkSize);

        ImGuiID dockMainId = dockSpaceId;
        ImGuiID dockIdRight   = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Right, 0.25f, nullptr, &dockMainId);
        ImGuiID dockIdLeft    = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Left, 0.20f, nullptr, &dockMainId);
        ImGuiID dockIdBottom  = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Down, 0.25f, nullptr, &dockMainId);
        ImGuiID dockIdCenter  = dockMainId;

        ImGuiID dockIdLeftTop;
        ImGuiID dockIdLeftBottom = ImGui::DockBuilderSplitNode(dockIdLeft, ImGuiDir_Down, 0.3f, &dockIdLeftTop, &dockIdLeft);
        
        ImGui::DockBuilderDockWindow("Scene hierarchy", dockIdLeft);
        ImGui::DockBuilderDockWindow("Properties", dockIdRight);
        ImGui::DockBuilderDockWindow("Objects", dockIdBottom);
        ImGui::DockBuilderDockWindow("Benchmark", dockIdLeftBottom);

        // ImGui::DockBuilderDockWindow("Something", dockIdCenter);

        ImGui::DockBuilderFinish(dockSpaceId);
    }


    ImGui::End();
}

void DebugEditor::showDebugInfo()
{
    BeginDockSpace();
    showAllObjectsInTheScene();
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
    ImGui::Begin("Properties");

    if (!gameObject)
    {
        ImGui::End();
        return;
    }

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


            if (auto cube = dynamic_cast<geometries::Cube*>(m_selectedGameObject))
            {
                auto model = cube->getModel();

                if (model)
                {
                    for (unsigned int meshIndex = 0; meshIndex < model->getMeshes().size(); meshIndex++)
                    {
                        auto& mesh = model->getMeshes()[meshIndex];

                        auto material = mesh.getMaterial();

                        if (!material)
                            continue;

                        std::string header = "Mesh " + std::to_string(meshIndex);

                        if (ImGui::CollapsingHeader(header.c_str()))
                        {
                            glm::vec3 color = material->getBaseColor();

                            if (ImGui::ColorEdit3(("Base Color##" + std::to_string(meshIndex)).c_str(), &color.x))
                            {
                                material->setBaseColor(color);
                            }

                            ImGui::SeparatorText("Textures");

                            for (auto textureType : {GLitch::Texture::TextureType::Diffuse, GLitch::Texture::TextureType::Normal,
                                                     GLitch::Texture::TextureType::Metallic, GLitch::Texture::TextureType::Roughness,
                                                     GLitch::Texture::TextureType::AO})
                            {
                                GLitch::Texture* tex = material->getTexture(textureType);

                                if (tex)
                                {
                                    ImGui::Text("%s: %s", ::fromTypeToString(textureType).c_str(), tex->getName().c_str());
                                }
                                else
                                {
                                    ImGui::Text("%s: (none)", ::fromTypeToString(textureType).c_str());
                                }
                            }
                        }
                    }
                }
            }
            else if (auto skeleton = dynamic_cast<Void*>(m_selectedGameObject))
            {
                auto model = skeleton->getModel();



                if (model)
                {
                    for (unsigned int meshIndex = 0; meshIndex < model->getMeshes().size(); meshIndex++)
                    {
                        auto& mesh = model->getMeshes()[meshIndex];

                        auto material = mesh.getMaterial();

                        if (!material)
                            continue;

                        std::string header = "Mesh " + std::to_string(meshIndex);

                        if (ImGui::CollapsingHeader(header.c_str()))
                        {
                            glm::vec3 color = material->getBaseColor();

                            if (ImGui::ColorEdit3(("Base Color##" + std::to_string(meshIndex)).c_str(), &color.x))
                            {
                                material->setBaseColor(color);
                            }

                            ImGui::SeparatorText("Textures");

                            for (auto textureType : {GLitch::Texture::TextureType::Diffuse, GLitch::Texture::TextureType::Normal,
                                                     GLitch::Texture::TextureType::Metallic, GLitch::Texture::TextureType::Roughness,
                                                     GLitch::Texture::TextureType::AO})
                            {
                                GLitch::Texture* tex = material->getTexture(textureType);

                                if (tex)
                                {
                                    ImGui::Text("%s: %s", ::fromTypeToString(textureType).c_str(), tex->getName().c_str());
                                }
                                else
                                {
                                    ImGui::Text("%s: (none)", ::fromTypeToString(textureType).c_str());
                                }
                            }
                        }
                    }
                }
            }



            std::vector<std::string> allModelNames;
            std::vector<const char*> convertedModelNames;
            std::string currentModelName;

            if (auto skeleton = dynamic_cast<Void*>(m_selectedGameObject); skeleton)
            {
                currentModelName = skeleton->getModel()->getName();
                allModelNames = AssetsManager::instance().getAllSkinnedModelsNames();
            }
            else if (auto cube = dynamic_cast<geometries::Cube*>(m_selectedGameObject); cube)
            {
                currentModelName = cube->getModel()->getName();
                allModelNames = AssetsManager::instance().getAllStaticModelsNames();
            }

             for (const auto& modelName : allModelNames)
                convertedModelNames.push_back(modelName.c_str());

            ImGui::Text("Model %s", currentModelName.c_str());
            ImGui::SameLine();

            auto i = std::ranges::find_if(convertedModelNames,
        [&currentModelName](const std::string& modelName){return currentModelName == modelName;});

            static int selectedModel = std::distance(convertedModelNames.begin(), i);

            if (ImGui::Combo("##Model combo", &selectedModel, convertedModelNames.data(), static_cast<int>(convertedModelNames.size())))
            {
                if (auto skeleton = dynamic_cast<Void*>(m_selectedGameObject); skeleton)
                {
                    if (auto m = AssetsManager::instance().getSkinnedModelByName(convertedModelNames[selectedModel]); m)
                        skeleton->setModel(m);
                }
                else if (auto cube = dynamic_cast<geometries::Cube*>(m_selectedGameObject); cube)
                {
                    if (auto m = AssetsManager::instance().getStaticModelByName(convertedModelNames[selectedModel]); m)
                        cube->setModel(m);
                }
            }

            ImGui::EndTabItem();
        }

        if (auto skeleton = dynamic_cast<Void*>(m_selectedGameObject); skeleton && ImGui::BeginTabItem("Bones"))
        {
            displayBonesHierarchy(skeleton->getModel() ? &skeleton->getModel()->getSkeleton() : nullptr);
            ImGui::EndTabItem();

            if (ImGui::BeginTabItem("Animation"))
            {
                for (const auto& anim : skeleton->getModel()->getAnimations())
                    ImGui::Text("Animation %s", anim.name.c_str());

                ImGui::EndTabItem();
            }
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
