#ifndef MOUSE_HPP
#define MOUSE_HPP

#include <GLFW/glfw3.h>

namespace input
{

enum class MouseButton
{
    LEFT = GLFW_MOUSE_BUTTON_LEFT,
    RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
    NONE = - 1
};

class MouseManager
{
public:
    MouseManager();
    // void init();
    static MouseManager& instance();
    static void mouseCallback(GLFWwindow* window, double x, double y);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    [[nodiscard]] double getX() const;
    [[nodiscard]] double getY() const;

    [[nodiscard]] bool isLeftButtonPressed() const;
    [[nodiscard]] bool isRightButtonPressed() const;

    [[nodiscard]] bool isLeftButtonReleased();
    [[nodiscard]] bool isRightButtonReleased();

    GLFWwindow *window{nullptr};
private:
    double m_x{0.0f};
    double m_y{0.0f};

    MouseButton m_heldButton{MouseButton::NONE};
    MouseButton m_prevHeldButton{MouseButton::NONE};

    bool m_leftButtonPressed = false;
    bool m_rightButtonPressed = false;
};

static MouseManager& Mouse = MouseManager::instance();

}; //namespace input

#endif //MOUSE_HPP