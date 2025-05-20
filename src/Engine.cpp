#include "Engine.hpp"
#include "glad.h"

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
#include "Renderer.hpp"
#include "SceneManager.hpp"
#include "ShaderManager.hpp"
#define IMGUI_ENABLE_DOCKING

bool Engine::run()
{
    try
    {
        init();
    }
    catch (const std::exception &e)
    {
        std::cerr << "ENGINE_RUN_ERROR: COULD NOT INITIALIZE ENGINE: " << e.what() << std::endl;
        return false;
    }

    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    while (window::WindowsManager::instance().getCurrentWindow()->isWindowOpened())
    {
        const float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        Renderer::instance().beginFrame();
        CameraManager::getInstance().getActiveCamera()->update(deltaTime);
        physics::PhysicsController::instance().simulate(deltaTime);
        SceneManager::instance().updateCurrentScene(deltaTime);
        DebugEditor::instance().update();
        Renderer::instance().endFrame();

        glfwSwapBuffers(window::WindowsManager::instance().getCurrentWindow()->getOpenGLWindow());
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return true;
}

void Engine::init()
{
    if (!glfwInit())
        throw std::runtime_error("Engine::init(): Failed to initialize glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    window::WindowsManager::instance().setCurrentWindow(window::WindowsManager::instance().createWindow());

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw std::runtime_error("Engine::init(): Failed to initialize GLAD");

    CameraManager::getInstance().setActiveCamera(CameraManager::getInstance().createCamera());
    physics::PhysicsController::instance().init();

    auto mainWindow = window::WindowsManager::instance().getCurrentWindow();
    glfwSetKeyCallback(mainWindow->getOpenGLWindow(), input::KeysManager::keyCallback);
    glfwSetMouseButtonCallback(mainWindow->getOpenGLWindow(), input::MouseManager::mouseButtonCallback);
    glfwSetCursorPosCallback(mainWindow->getOpenGLWindow(), input::MouseManager::mouseCallback);
    glfwSetInputMode(mainWindow->getOpenGLWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // #define GLFW_CURSOR_NORMAL          0x00034001
    // #define GLFW_CURSOR_HIDDEN          0x00034002
    // #define GLFW_CURSOR_DISABLED        0x00034003
    // #define GLFW_CURSOR_CAPTURED        0x00034004

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow->getOpenGLWindow(), &bufferWidth, &bufferHeight);
	Renderer::instance().initFrameBuffer(mainWindow->getWidth(), mainWindow->getHeight());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(mainWindow->getOpenGLWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = 1.0;
    style.WindowRounding = 3;
    style.GrabRounding = 1;
    style.GrabMinSize = 20;
    style.FrameRounding = 3;


    style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glViewport(0, 0, bufferWidth, bufferHeight);

    AssetsManager::instance().preLoadPathsForAllModels();
    AssetsManager::instance().preLoadPathsForAllTextures();
    AssetsManager::instance().preLoadPathsForAllMaterials();
    ShaderManager::instance().preLoadShaders();

    auto loadingScene = new LoadingScene();
    loadingScene->create();
    SceneManager::instance().addScene(loadingScene);
    SceneManager::instance().addScene(new DefaultScene());
    SceneManager::instance().setCurrentScene(loadingScene);
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
