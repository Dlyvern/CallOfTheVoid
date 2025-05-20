#ifndef SCRIPTS_REGISTER_HPP
#define SCRIPTS_REGISTER_HPP

#include "ScriptComponent.hpp"
#include <functional>
#include <memory>

class ScriptsRegister
{
public:
    static ScriptsRegister& instance();

    void registerScript(const std::string& name, const std::function<std::shared_ptr<ScriptComponent>()>& func);

    std::shared_ptr<ScriptComponent> createScript(const std::string& scriptName);

private:
    std::unordered_map<std::string, std::function<std::shared_ptr<ScriptComponent>()>> m_scripts;
};

#endif //SCRIPTS_REGISTER_HPP
