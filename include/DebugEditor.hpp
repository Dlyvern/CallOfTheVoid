#ifndef DEBUG_EDITOR_HPP
#define DEBUG_EDITOR_HPP

#include "GameObject.hpp"
#include "Skeleton.hpp"
#include <filesystem>
#include "Filesystem.hpp"
#include "ActionsManager.hpp"

class DebugEditor
{
public:
    static DebugEditor& instance();
    void update();
    ~DebugEditor();
    [[nodiscard]] bool getDebugMode() const;

private:

    struct DraggingInfo final
    {
    public:
        std::string name;
    };

    void showViewPort();
    void showDebugInfo();
    void showObjectInfo();
    void showProperties();
    void showMaterialInfo();
    void showAllObjectsInTheScene();
    void showAssetsInfo();
    void showGuizmosInfo();

    void displayBonesHierarchy(Skeleton* skeleton, common::BoneInfo* parent = nullptr);

    void updateInput();

    std::filesystem::path m_assetsPath{filesystem::getResourcesFolderPath()};

    DebugEditor();
    DebugEditor(const DebugEditor&) = delete;
    DebugEditor& operator=(const DebugEditor&) = delete;
    DebugEditor(DebugEditor&&) = delete;
    DebugEditor& operator=(DebugEditor&&) = delete;

    DraggingInfo m_draggingInfo;

    GameObject* m_selectedGameObject{nullptr};
    Material* m_selectedMaterial{nullptr};

    ActionsManager m_actionsManager;

    std::shared_ptr<GameObject> m_savedGameObject{nullptr};

    std::vector<GameObject*> m_selectedGameObjects;

    bool m_isDebugMode{false};

    enum class TransformMode
    {
        Translate,
        Rotate,
        Scale
    };

    TransformMode m_transformMode{TransformMode::Translate};

    int m_selectedModelIndex{-1};
};

#endif //DEBUG_EDITOR_HPP
