#include "MainWindow.hpp"

#include <stdexcept>

window::MainWindow::MainWindow()
{
    m_window = glfwCreateWindow(m_currentWindowData.width, m_currentWindowData.height, m_windowName.c_str(), glfwGetPrimaryMonitor(), nullptr);
    
    if (!m_window)
    {
        glfwTerminate();
        throw std::runtime_error("MainWindow::MainWindow(): could not create a window");
    }

    glfwMakeContextCurrent(m_window);

    glViewport(0, 0, m_currentWindowData.width, m_currentWindowData.height);
}

bool window::MainWindow::isWindowOpened()
{
    return !(glfwWindowShouldClose(m_window));
}

int window::MainWindow::getHeight() const
{
    return m_currentWindowData.height;
}

int window::MainWindow::getWidth() const
{
    return m_currentWindowData.width;
}

window::WindowData window::MainWindow::getWindowData() const
{
    return m_currentWindowData;
}

GLFWwindow *window::MainWindow::getOpenGLWindow() const
{
    return m_window;
}