#include "game_window.h"
#include "graphics_backend_api.h"
#include "top_menu_bar.h"
#include "window_manager.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <utility>
#include <GLFW/glfw3.h>

RenderHandler s_RenderHandler;
KeyboardInputHandlerDelegate s_KeyboardInputHandler;
MouseMoveHandlerDelegate s_MouseMoveHandler;

void KeyboardFunction(GLFWwindow* window, int keycode, int scancode, int action, int mods);
void MouseMoveFunction(GLFWwindow *window, double x, double y);

GameWindow::GameWindow(int width,
                       int height,
                       RenderHandler renderHandler,
                       KeyboardInputHandlerDelegate keyboardInputHandler,
                       MouseMoveHandlerDelegate mouseMoveHandler)
{
    s_RenderHandler = std::move(renderHandler);
    s_KeyboardInputHandler = std::move(keyboardInputHandler);
    s_MouseMoveHandler = std::move(mouseMoveHandler);

    if (!glfwInit())
    {
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GraphicsBackend::GetMajorVersion());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GraphicsBackend::GetMinorVersion());
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_WindowPtr = glfwCreateWindow(width, height, "RenderEngine", nullptr, nullptr);
    if (m_WindowPtr == nullptr)
    {
        return;
    }

    glfwMakeContextCurrent(m_WindowPtr);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    glfwSetCursorPosCallback(m_WindowPtr, MouseMoveFunction);
    glfwSetKeyCallback(m_WindowPtr, KeyboardFunction);

    ImGui_ImplGlfw_InitForOpenGL(m_WindowPtr, true);
    ImGui_ImplOpenGL3_Init(GraphicsBackend::GetShadingLanguageDirective().c_str());
}

GameWindow::~GameWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_WindowPtr);
    glfwTerminate();
}

void GameWindow::BeginMainLoop()
{
    while (!glfwWindowShouldClose(m_WindowPtr))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int width, height;
        glfwGetFramebufferSize(m_WindowPtr, &width, &height);

        if (s_RenderHandler)
        {
            s_RenderHandler(width, height);
        }

        TopMenuBar::Draw([this]() { SetCloseFlag(); });
        WindowManager::DrawAllWindows();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_WindowPtr);
    }
}

void GameWindow::SetCloseFlag()
{
    glfwSetWindowShouldClose(m_WindowPtr, true);
}

void KeyboardFunction(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
    auto &io = ImGui::GetIO();
    if (io.WantCaptureKeyboard || (action != GLFW_PRESS && action != GLFW_RELEASE))
    {
        return;
    }

    if (s_KeyboardInputHandler)
    {
        s_KeyboardInputHandler(static_cast<char>(keycode), action == GLFW_PRESS);
    }
}

void MouseMoveFunction(GLFWwindow *window, double x, double y)
{
    auto &io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
    {
        s_MouseMoveHandler(x, y);
    }
}