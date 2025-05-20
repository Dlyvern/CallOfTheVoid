#ifndef SCRIPT_COMPONENT_HPP
#define SCRIPT_COMPONENT_HPP

#include <memory>

#include "Component.hpp"
#include <string>
#include <vector>

class ScriptComponent : public Component
{
public:
    ScriptComponent() = default;
    explicit ScriptComponent(const std::string& scriptName);

    virtual void update(float deltaTime)
    {
        for (auto& script : m_scripts)
            script->update(deltaTime);
    }

    void addScript(std::shared_ptr<ScriptComponent> script)
    {
        script->setOwner(getOwner());
        m_scripts.push_back(script);
    }

    [[nodiscard]] const std::string& getScriptName() const;

    template<typename T>
    T* getScript()
    {
        for (auto& script : m_scripts)
            if (auto* casted = dynamic_cast<T*>(script.get()))
                return casted;

        return nullptr;
    }

    const std::vector<std::shared_ptr<ScriptComponent>>& getScripts() const { return m_scripts; }

    virtual ~ScriptComponent();
private:
    std::string m_scriptName;
    std::vector<std::shared_ptr<ScriptComponent>> m_scripts;
};

#endif //SCRIPT_COMPONENT_HPP
