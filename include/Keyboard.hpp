#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#pragma once

#include <unordered_map>
#include <GLFW/glfw3.h>

namespace input
{
enum class KeyCode : int
{
    W = GLFW_KEY_W,
    A = GLFW_KEY_A,
    S = GLFW_KEY_S,
    D = GLFW_KEY_D,
    SPACE = GLFW_KEY_SPACE,
    E = GLFW_KEY_E,
};

class KeysManager
{
public:
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    bool isKeyPressed(KeyCode keyCode);

    static KeysManager& instance();
private:
    std::unordered_map<KeyCode, bool> m_keys
    {
        {input::KeyCode::W, false},
    };

    void keyPressed(KeyCode keyCode);
    void keyReleased(KeyCode keyCode);
};

static KeysManager& Keyboard = KeysManager::instance();

} //namespace input

#endif //KEYBOARD_HPP