#include "Mouse.hpp"
#include <iostream>

input::MouseManager &input::MouseManager::instance()
{
    static MouseManager mouseManager;
    return mouseManager;
}

void input::MouseManager::mouseCallback(GLFWwindow *window, double x, double y)
{
    // input::Mouse.m_x = x;
    // input::Mouse.m_y = y;
    Mouse.window = window;
    glfwGetCursorPos(window, &input::Mouse.m_x, &input::Mouse.m_y);
}

double input::MouseManager::getX() const
{
    return m_x;
}

double input::MouseManager::getY() const
{
    return m_y;
}