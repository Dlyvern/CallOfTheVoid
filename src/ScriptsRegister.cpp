#include "ScriptsRegister.hpp"

ScriptsRegister& ScriptsRegister::instance()
{
    static ScriptsRegister instance;
    return instance;
}

void ScriptsRegister::registerScript(const std::string& name, const std::function<std::shared_ptr<ScriptComponent>()> &func)
{
    m_scripts[name] = func;
}

std::shared_ptr<ScriptComponent> ScriptsRegister::createScript(const std::string &scriptName)
{
    const auto script = m_scripts.find(scriptName);
    return script == m_scripts.end() ? nullptr : script->second();
}
