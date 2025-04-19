#include "DebugEditor.hpp"

#include <glad.h>
#include <imgui.h>

#include "CameraManager.hpp"
#include "DebugLine.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "Raycasting.hpp"
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
    if (!m_selectedGameObject)
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
            ImGui::InputFloat3("Rotation", &rotation[0]);

            glm::vec3 scale = m_selectedGameObject->getScale();
            ImGui::Text("Scale");
            ImGui::InputFloat3("Scale", &scale[0]);

            ImGui::EndTabItem();
        }

        if (auto skeleton = dynamic_cast<Void*>(m_selectedGameObject); skeleton && ImGui::BeginTabItem("Bones"))
        {
            displayBonesHierarchy(skeleton->getModel() ? &skeleton->getModel()->getSkeleton() : nullptr);
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
        {
            auto bo = skeleton->getBone(childBone);
            displayBonesHierarchy(skeleton, bo);
        }

        ImGui::TreePop();
    }
}

DebugEditor::DebugEditor() = default;
