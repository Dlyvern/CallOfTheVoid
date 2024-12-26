#include "../libraries/glad/glad.h"

#include "Engine.hpp"

#include <iostream>

#include "DefaultScene.hpp"
#include "LoadingScene.hpp"

#include "AssetsManager.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

//TODO: MAKE CURRENT SCENE MORE USER FRIENDLY
//Current level/scene system or something like that

void Engine::run()
{
    init();

    GLfloat lastFrame = 0.0f;
    GLfloat deltaTime = 0.0f;
    
    while(m_mainWindow->isWindowOpened())
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if((*m_currentScene)->isOver() && m_currentScene + 1 != m_allScenes.end())
        {
            ++m_currentScene;
            (*m_currentScene)->create();
        }

        if((*m_currentScene))
            (*m_currentScene)->update(deltaTime);

        glfwSwapBuffers(m_mainWindow->getOpenGLWindow());
        glfwPollEvents();
    }
}

Engine &Engine::instance()
{
    static Engine engine{};

    return engine;
}

void Engine::init()
{
    if (!glfwInit())
        throw std::runtime_error("Engine::init(): Failed to initialize glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    m_mainWindow = std::make_shared<Window::MainWindow>();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Engine::init(): Failed to initialize GLAD");

    glfwSetKeyCallback(m_mainWindow->getOpenGLWindow(), input::KeysManager::keyCallback);
    glfwSetCursorPosCallback(m_mainWindow->getOpenGLWindow(), input::MouseManager::mouseCallback);
    
    glfwSetInputMode(m_mainWindow->getOpenGLWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_allScenes.emplace_back(std::make_shared<LoadingScene>());
    m_allScenes.emplace_back(std::make_shared<DefaultScene>());

    m_currentScene = m_allScenes.begin();

    AssetsManager::instance().initMinimim();

    // AssetsManager::instance().loadTextures();
    AssetsManager::instance().loadModels();

    (*m_currentScene)->create();

}

void Engine::glCheckError(const char *file, int line)
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
        }

        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
}
