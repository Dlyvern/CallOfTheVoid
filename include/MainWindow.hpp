#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <GLFW/glfw3.h>
#include <string>

namespace window
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

    constexpr WindowData FULLSCREEN_WINDOW_DATA{1920, 1080, WindowMode::FULLSCREEN};
    constexpr WindowData WINDOWED_WINDOW_DATA{800, 600, WindowMode::WINDOWED};

    class MainWindow
    {
    public:
        MainWindow();

        bool isWindowOpened();

        [[nodiscard]] int getHeight() const;
        [[nodiscard]] int getWidth() const;

        [[nodiscard]] WindowData getWindowData() const;

        [[nodiscard]] GLFWwindow* getOpenGLWindow() const;

    private:
        GLFWwindow* m_window{nullptr};
        WindowData m_currentWindowData{window::FULLSCREEN_WINDOW_DATA};
        bool m_hasFocus;
        
        const std::string m_windowName{"CallOfTheVoid"};
    };
}

#endif //MAIN_WINDOW_HPP