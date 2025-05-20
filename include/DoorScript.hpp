#ifndef DOOR_SCRIPT_HPP
#define DOOR_SCRIPT_HPP

#include "ScriptComponent.hpp"

#include "Interactable.hpp"

class DoorScript final : public ScriptComponent, public Interactable
{
public:
    DoorScript();
    void update(float deltaTime) override;
    void interact() override;
};


#endif //DOOR_SCRIPT_HPP
