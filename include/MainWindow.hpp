#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <GLFW/glfw3.h>
#include <string>

namespace Window
{
    enum class WindowMode : uint8_t
    {
        FULLSCREEN = 0,
        WINDOWED = 1
    };

    struct WindowData
    {
    public:
        int width{0};
        int height{0};
        WindowMode windowMode;
    };

    const WindowData FULLSCREEN_WINDOW_DATA{1920, 1080, WindowMode::FULLSCREEN};
    const WindowData WINDOWED_WINDOW_DATA{800, 600, WindowMode::WINDOWED};

    class MainWindow
    {
    public:
        MainWindow();

        bool isWindowOpened();

        GLFWwindow* getOpenGLWindow() const;

    private:
        GLFWwindow* m_window{nullptr};
        WindowData m_currentWindowData{Window::FULLSCREEN_WINDOW_DATA};
        bool m_hasFocus;
        
        const std::string m_windowName{"CallOfTheVoid"};

        static void frameBufferSizeCallback(GLFWwindow* Window, int width, int height);
        static void windowFocusCallback(GLFWwindow* window, int isFocused);
    };
}

#endif //MAIN_WINDOW_HPP