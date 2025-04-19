#include "Keyboard.hpp"

#include <iostream>

void input::KeysManager::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto keyCode = static_cast<input::KeyCode>(key);

    if(action == GLFW_PRESS)
    {
        Keyboard.keyPressed(keyCode);
    }
    else if(action == GLFW_RELEASE)
    {
        Keyboard.keyReleased(keyCode);
    }
}

bool input::KeysManager::isKeyPressed(KeyCode keyCode)
{
    auto it = m_keys.find(keyCode);
    return it != m_keys.end() && it->second;
}

bool input::KeysManager::isKeyReleased(KeyCode keyCode)
{
    if (const auto it = m_keys.find(keyCode); it != m_keys.end() && !it->second && m_prevKeys[keyCode])
    {
        m_prevKeys[keyCode] = false;
        return true;
    }
    return false;

}

input::KeysManager &input::KeysManager::instance()
{
    static input::KeysManager keyboard;
    return keyboard;
}

void input::KeysManager::keyPressed(KeyCode keyCode)
{
    m_keys[keyCode] = true;
    m_prevKeys[keyCode] = true;
}

void input::KeysManager::keyReleased(KeyCode keyCode)
{
    m_keys[keyCode] = false;
}
