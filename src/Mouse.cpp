#include "Mouse.hpp"

input::MouseManager &input::MouseManager::instance()
{
    static MouseManager mouseManager;
    return mouseManager;
}

void input::MouseManager::mouseCallback(GLFWwindow *window, double x, double y)
{
    input::Mouse.m_x = x;
    input::Mouse.m_y = y;
}

float input::MouseManager::getX() const
{
    return m_x;
}

float input::MouseManager::getY() const
{
    return m_y;
}