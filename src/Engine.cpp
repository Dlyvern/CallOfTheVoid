#include "glad.h"

#include "Engine.hpp"

#include <iostream>

#include "DefaultScene.hpp"
#include "LoadingScene.hpp"

#include "AssetsManager.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

#include "CameraManager.hpp"
#include "DebugTextHolder.hpp"
#include "Physics.hpp"
#include "WindowsManager.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "DebugEditor.hpp"

Engine &Engine::instance()
{
    static Engine engine{};

    return engine;
}


void Engine::run()
{
    init();

    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    while(m_mainWindow->isWindowOpened())
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if((*m_currentScene)->isOver() && m_currentScene + 1 != m_allScenes.end())
        {
            ++m_currentScene;
            (*m_currentScene)->create();
        }

        if((*m_currentScene))
        {
            CameraManager::getInstance().getActiveCamera()->update(deltaTime);
            physics::PhysicsController::instance().simulate(deltaTime);
            DebugEditor::instance().update();
            // debug::DebugTextHolder::instance().update(deltaTime);
            (*m_currentScene)->update(deltaTime);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_mainWindow->getOpenGLWindow());
        glfwPollEvents();
    }
}

void Engine::init()
{
    if (!glfwInit())
        throw std::runtime_error("Engine::init(): Failed to initialize glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    window::WindowsManager::instance().setCurrentWindow(window::WindowsManager::instance().createWindow());

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw std::runtime_error("Engine::init(): Failed to initialize GLAD");

    CameraManager::getInstance().setActiveCamera(CameraManager::getInstance().createCamera());
    physics::PhysicsController::instance().init();

    m_mainWindow = window::WindowsManager::instance().getCurrentWindow();
    glfwSetKeyCallback(m_mainWindow->getOpenGLWindow(), input::KeysManager::keyCallback);
    glfwSetMouseButtonCallback(m_mainWindow->getOpenGLWindow(), input::MouseManager::mouseButtonCallback);
    glfwSetCursorPosCallback(m_mainWindow->getOpenGLWindow(), input::MouseManager::mouseCallback);
    glfwSetInputMode(m_mainWindow->getOpenGLWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // #define GLFW_CURSOR_NORMAL          0x00034001
    // #define GLFW_CURSOR_HIDDEN          0x00034002
    // #define GLFW_CURSOR_DISABLED        0x00034003
    // #define GLFW_CURSOR_CAPTURED        0x00034004


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    const char* glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(m_mainWindow->getOpenGLWindow(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // input::MouseManager::instance().init();

    AssetsManager::instance().preLoadPathsForAllModels();
    AssetsManager::instance().preLoadPathsForAllTextures();

    m_allScenes.emplace_back(std::make_shared<LoadingScene>());
    m_allScenes.emplace_back(std::make_shared<DefaultScene>());

    m_currentScene = m_allScenes.begin();

    (*m_currentScene)->create();
}

void Engine::glCheckError(const char *file, const int line)
{
    GLenum errorCode;
    
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;

        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            default:                               error = "UNDEFINED_ERROR"; break;
        }

        const std::string errorText = error + " | " + file + " (" + std::to_string(line) + ")";

        std::cout << errorText << std::endl;

        debug::DebugTextHolder::instance().addText(errorText);
    }
}
