#include "DoorScript.hpp"
#include <iostream>

#include "AnimatorComponent.hpp"
#include "AssetsManager.hpp"
#include "GameObject.hpp"
#include "Macros.hpp"
// #include "PluginManager.hpp"

static common::Animation animation;

DoorScript::DoorScript() : ScriptComponent("DoorScript")
{
    animation = AssetsManager::instance().loadAnimationFromJson(filesystem::getAnimationsFolderPath().string() + "/door_open.anim");
}

void DoorScript::update(float deltaTime)
{
}

void DoorScript::interact()
{
    if (auto owner = this->getOwner())
    {
        if (owner->hasComponent<AnimatorComponent>())
        {
            animation.gameObject = owner;
            owner->getComponent<AnimatorComponent>()->playAnimation(&animation, false);
        }
    }
}

REGISTER_SCRIPT(DoorScript)


// extern "C"
// {
//     void registerScripts(PluginManager& pluginManager)
//     {
//         pluginManager.registerScript("DoorScript", []() -> void* {
//             return new DoorScript();
//         });
//     }
// }