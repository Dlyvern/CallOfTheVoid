#include "Renderer.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include "CameraManager.hpp"
#include "WindowsManager.hpp"

Renderer& Renderer::instance()
{
    static Renderer self;
    return self;
}

void Renderer::beginFrame()
{
    const auto* currentWindow = window::WindowsManager::instance().getCurrentWindow();
    const float aspect = static_cast<float>(currentWindow->getWidth()) / static_cast<float>(currentWindow->getHeight());
    m_frameData.projectionMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    const auto* activeCamera = CameraManager::getInstance().getActiveCamera();
    m_frameData.viewMatrix = activeCamera->getViewMatrix();
    m_frameData.cameraPosition = activeCamera->getPosition();
}

const RendererFrameData& Renderer::getFrameData() const
{
    return m_frameData;
}
