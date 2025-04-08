#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <memory>
#include <vector>
#include "MainWindow.hpp"
#include "Scene.hpp"

class Engine
{
public:
    void run();

    static Engine& instance();

private:
    void init();

    void glCheckError(const char* file, int line);

    std::vector<std::shared_ptr<Scene>>::iterator m_currentScene;
    std::vector<std::shared_ptr<Scene>> m_allScenes;

    window::MainWindow* m_mainWindow{nullptr};
};

#define callGLCheckError() glCheckError(__FILE__, __LINE__)

#endif //ENGINE_HPP