#include "TestScript.hpp"

void TestScript::interact()
{
    std::cout << "TestScript::interact" << std::endl;
}

REGISTER_SCRIPT(TestScript)
