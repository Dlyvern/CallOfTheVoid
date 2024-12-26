#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <memory>
#include <vector>
#include <iostream>

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

    std::shared_ptr<Window::MainWindow> m_mainWindow{nullptr};
};

#define callGLCheckError() glCheckError(__FILE__, __LINE__)

#endif //ENGINE_HPP