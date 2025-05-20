#include "PluginManager.hpp"
#include <dlfcn.h>

bool PluginManager::loadPlugin(const std::string &pluginPath)
{
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);

    if (!handle)
        return false;

    if (auto registerFunc = reinterpret_cast<void(*)(PluginManager&)>(dlsym(handle, "registerScripts")))
    {
        registerFunc(*this);
        return true;
    }

    return false;

}

void PluginManager::unloadPlugin(const std::string &pluginName)
{
    // dlclose()
}

void PluginManager::registerScript(const std::string &scriptName, ScriptFactory factory)
{
    m_registeredScripts[scriptName] = factory;
}

PluginManager::ScriptFactory PluginManager::getScriptFactory(const std::string &scriptName)
{
    return m_registeredScripts.at(scriptName);
}
