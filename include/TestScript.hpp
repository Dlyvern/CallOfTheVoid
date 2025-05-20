#ifndef TESTSCRIPT_HPP
#define TESTSCRIPT_HPP

#include <iostream>

#include "ScriptComponent.hpp"

#include "Interactable.hpp"

#include "Macros.hpp"

class TestScript final : public ScriptComponent, public Interactable
{
public:
    TestScript() : ScriptComponent("TestScript") {}
    void update(float deltaTime) override {}
    void interact() override;
};


#endif //TESTSCRIPT_HPP
