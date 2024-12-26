#include "MainWindow.hpp"

#include <stdexcept>

Window::MainWindow::MainWindow()
{
    m_window = glfwCreateWindow(m_currentWindowData.width, m_currentWindowData.height, m_windowName.c_str(), glfwGetPrimaryMonitor(), nullptr);
    
    if (!m_window)
    {
        glfwTerminate();
        throw std::runtime_error("MainWindow::MainWindow(): could not create a window");
    }

    glfwMakeContextCurrent(m_window);

    glfwSetFramebufferSizeCallback(m_window, frameBufferSizeCallback);
    glfwSetWindowFocusCallback(m_window, windowFocusCallback);

    glViewport(0, 0, m_currentWindowData.width, m_currentWindowData.height);
}

bool Window::MainWindow::isWindowOpened()
{
    return !(glfwWindowShouldClose(m_window));
}

GLFWwindow *Window::MainWindow::getOpenGLWindow() const
{
    return m_window;
}

void Window::MainWindow::frameBufferSizeCallback(GLFWwindow *Window, int width, int height)
{
}

void Window::MainWindow::windowFocusCallback(GLFWwindow *window, int isFocused)
{
    // m_hasFocus = isFocused;
}
