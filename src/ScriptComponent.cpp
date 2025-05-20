#include "ScriptComponent.hpp"

ScriptComponent::ScriptComponent(const std::string &scriptName) : m_scriptName(scriptName)
{
}

const std::string& ScriptComponent::getScriptName() const
{
    return m_scriptName;
}

ScriptComponent::~ScriptComponent() = default;
