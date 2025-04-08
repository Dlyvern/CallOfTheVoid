#ifndef WINDOWS_MANAGER_HPP
#define WINDOWS_MANAGER_HPP

#include "MainWindow.hpp"

namespace window
{
    class WindowsManager
    {
    public:
        static WindowsManager& instance();

        void setCurrentWindow(window::MainWindow* window);

        window::MainWindow* getCurrentWindow();

        window::MainWindow* createWindow();

    private:
        WindowsManager() = default;
        WindowsManager(const WindowsManager&) = delete;
        WindowsManager& operator=(const WindowsManager&) = delete;

        window::MainWindow* m_currentWindow{nullptr};

        static void frameBufferSizeCallback(GLFWwindow* Window, int width, int height);
        static void windowFocusCallback(GLFWwindow* window, int isFocused);
    };
} //namespace window

#endif //WINDOWS_MANAGER_HPP
