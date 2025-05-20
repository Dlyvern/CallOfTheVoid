#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP
#include "Scene.hpp"

class SceneManager
{
public:
    static SceneManager& instance();

    void setCurrentScene(Scene* scene);

    Scene* getCurrentScene();

    void addScene(Scene* scene);

    void updateCurrentScene(float deltaTime);

    static void saveObjectsIntoFile(const std::vector<std::shared_ptr<GameObject>>& objects, const std::string& filePath);
    static std::vector<std::shared_ptr<GameObject>> loadObjectsFromFile(const std::string& filePath);

    ~SceneManager() = default;
private:
    Scene* m_currentScene{nullptr};
    std::vector<Scene*> m_scenes;

    void onSceneIsOver(Scene* scene);

    SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager(SceneManager&&) = delete;
    SceneManager& operator=(SceneManager&&) = delete;
};

#endif //SCENE_MANAGER_HPP
