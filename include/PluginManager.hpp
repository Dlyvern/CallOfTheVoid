#ifndef PLUGIN_MANAGER_HPP
#define PLUGIN_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <functional>

class PluginManager
{
public:
    using ScriptFactory = std::function<void*()>;
    bool loadPlugin(const std::string& pluginPath);
    void unloadPlugin(const std::string& pluginName);
    void registerScript(const std::string& scriptName, ScriptFactory factory);
    ScriptFactory getScriptFactory(const std::string& scriptName);
private:
    std::unordered_map<std::string, ScriptFactory> m_registeredScripts;
};

#endif //PLUGIN_MANAGER_HPP
