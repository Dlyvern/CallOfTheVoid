#ifndef DEBUG_EDITOR_HPP
#define DEBUG_EDITOR_HPP
#include "GameObject.hpp"
#include "Skeleton.hpp"

class DebugEditor
{
public:
    static DebugEditor& instance();
    void update();
    ~DebugEditor();
    bool getDebugMode();
private:
    void showDebugInfo();
    void showObjectInfo(GameObject* gameObject);
    void showAllObjectsInTheScene();

    void displayBonesHierarchy(Skeleton* skeleton, common::BoneInfo* parent = nullptr);

    DebugEditor();
    DebugEditor(const DebugEditor&) = delete;
    DebugEditor& operator=(const DebugEditor&) = delete;
    DebugEditor(DebugEditor&&) = delete;
    DebugEditor& operator=(DebugEditor&&) = delete;

    GameObject* m_selectedGameObject{nullptr};

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
