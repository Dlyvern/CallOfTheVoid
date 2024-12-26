#include "Engine.hpp"

int main()
{
    Engine::instance().run();
    
    glfwTerminate();
    
    return 0;
}