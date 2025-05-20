#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

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
    K = GLFW_KEY_K,
    O = GLFW_KEY_O,
    DELETE = GLFW_KEY_DELETE,
    R = GLFW_KEY_R,
    C = GLFW_KEY_C,
    LeftCtrl = GLFW_KEY_LEFT_CONTROL,
    RightCtrl = GLFW_KEY_RIGHT_CONTROL,
    Z = GLFW_KEY_Z,
    V = GLFW_KEY_V,
    Y = GLFW_KEY_Y,
};

class KeysManager
{
public:
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    bool isKeyPressed(KeyCode keyCode);
    bool isKeyReleased(KeyCode keyCode);

    static KeysManager& instance();
private:
    std::unordered_map<KeyCode, bool> m_keys;
    std::unordered_map<KeyCode, bool> m_prevKeys;


    void keyPressed(KeyCode keyCode);
    void keyReleased(KeyCode keyCode);
};

static KeysManager& Keyboard = KeysManager::instance();

} //namespace input

#endif //KEYBOARD_HPP