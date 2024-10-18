#include "engine_framework.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "graphics_backend_api.h"
#include "graphics_backend_implementations/graphics_backend_api_opengl.h"

#include <GLFW/glfw3.h>

GLFWwindow *s_Window = nullptr;

void KeyboardFunction(GLFWwindow *window, int keycode, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        EngineFramework::ProcessKeyPress(static_cast<char>(keycode), action == GLFW_PRESS);
    }
}

void MouseMoveFunction(GLFWwindow *window, double x, double y)
{
    EngineFramework::ProcessMouseMove(static_cast<float>(x), static_cast<float>(y));
}

int main(int argc, char **argv)
{
    if (!glfwInit())
    {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 1920;
    int height = 1080;

    s_Window = glfwCreateWindow(width, height, "RenderEngine", nullptr, nullptr);
    if (!s_Window)
    {
        return 1;
    }

    glfwMakeContextCurrent(s_Window);
    glfwSwapInterval(1);

    glfwSetCursorPosCallback(s_Window, MouseMoveFunction);
    glfwSetKeyCallback(s_Window, KeyboardFunction);

    EngineFramework::Initialize(nullptr, "OpenGL");

    ImGui_ImplGlfw_InitForOpenGL(s_Window, true);

    GraphicsBackendOpenGL* openGL = reinterpret_cast<GraphicsBackendOpenGL*>(GraphicsBackend::Current());
    ImGui_ImplOpenGL3_Init(openGL->GetGLSLVersionString().c_str());

    while (!glfwWindowShouldClose(s_Window))
    {
        if (EngineFramework::ShouldCloseWindow())
        {
            break;
        }

        glfwPollEvents();
        glfwGetFramebufferSize(s_Window, &width, &height);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        EngineFramework::TickMainLoop(nullptr, width, height);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(s_Window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    EngineFramework::Shutdown();

    glfwDestroyWindow(s_Window);
    glfwTerminate();

    return 0;
}
