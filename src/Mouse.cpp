#include "Mouse.hpp"
#include <iostream>
#include "DebugTextHolder.hpp"

// unsigned int t;
// unsigned int t2;

input::MouseManager::MouseManager()
{
}

// void input::MouseManager::init()
// {
//     t = debug::DebugTextHolder::instance().addText("");
//     t2 = debug::DebugTextHolder::instance().addText("");
// }

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


std::string mouseToString(input::MouseButton button)
{
    switch (button)
    {
        case input::MouseButton::LEFT:
            return "Left";
        case input::MouseButton::RIGHT:
            return "Right";
        case input::MouseButton::NONE:
            return "None";
    }
    return {""};
}

void input::MouseManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            Mouse.m_leftButtonPressed = true;
        } else if (action == GLFW_RELEASE) {
            Mouse.m_leftButtonPressed = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            Mouse.m_prevHeldButton = Mouse.m_heldButton;
            Mouse.m_rightButtonPressed = true;
            Mouse.m_heldButton = MouseButton::RIGHT;
        }
        else if (action == GLFW_RELEASE)
        {
            Mouse.m_prevHeldButton = Mouse.m_heldButton;
            Mouse.m_heldButton = MouseButton::NONE;
            Mouse.m_rightButtonPressed = false;
        }
    }

    // debug::DebugTextHolder::instance().changeText(t, "Prev held button " + mouseToString(Mouse.m_prevHeldButton));
    // debug::DebugTextHolder::instance().changeText(t2, "Held button " + mouseToString(Mouse.m_heldButton));
}

double input::MouseManager::getX() const
{
    return m_x;
}

double input::MouseManager::getY() const
{
    return m_y;
}

bool input::MouseManager::isLeftButtonPressed() const
{
    return m_leftButtonPressed;
}

bool input::MouseManager::isRightButtonPressed() const
{
    return m_rightButtonPressed;
}

bool input::MouseManager::isLeftButtonReleased()
{
    return !m_leftButtonPressed;
}

bool input::MouseManager::isRightButtonReleased()
{
    return m_heldButton == MouseButton::NONE && m_prevHeldButton == MouseButton::RIGHT;
}
