#ifndef MOUSE_HPP
#define MOUSE_HPP

#include <GLFW/glfw3.h>

namespace input
{

class MouseManager
{
public:
    static MouseManager& instance();
    static void mouseCallback(GLFWwindow* window, double x, double y);

    [[nodiscard]] double getX() const;
    [[nodiscard]] double getY() const;

    GLFWwindow *window{nullptr};
private:
    double m_x;
    double m_y;
};

static MouseManager& Mouse = MouseManager::instance();

}; //namespace input

#endif //MOUSE_HPP