#include "WindowsManager.hpp"

window::WindowsManager& window::WindowsManager::instance()
{
    static window::WindowsManager instance;
    return instance;
}

void window::WindowsManager::setCurrentWindow(window::MainWindow* window)
{
    m_currentWindow = window;

    glfwSetFramebufferSizeCallback(m_currentWindow->getOpenGLWindow(), frameBufferSizeCallback);
    glfwSetWindowFocusCallback(m_currentWindow->getOpenGLWindow(), windowFocusCallback);
}

window::MainWindow* window::WindowsManager::getCurrentWindow() const
{
    return m_currentWindow;
}

window::MainWindow* window::WindowsManager::createWindow()
{
    return new window::MainWindow();
}

void window::WindowsManager::frameBufferSizeCallback(GLFWwindow *Window, int width, int height)
{
}

void window::WindowsManager::windowFocusCallback(GLFWwindow *window, int isFocused)
{

}
