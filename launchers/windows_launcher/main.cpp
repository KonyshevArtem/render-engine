#include "engine_framework.h"
#include "graphics_backend_api.h"

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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GraphicsBackend::GetMajorVersion());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GraphicsBackend::GetMinorVersion());
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

    EngineFramework::Initialize(s_Window);

    while (!glfwWindowShouldClose(s_Window))
    {
        if (EngineFramework::ShouldCloseWindow())
        {
            break;
        }

        glfwPollEvents();
        glfwGetFramebufferSize(s_Window, &width, &height);

        EngineFramework::TickMainLoop(width, height);

        glfwSwapBuffers(s_Window);
    }

    EngineFramework::Shutdown();

    glfwDestroyWindow(s_Window);
    glfwTerminate();

    return 0;
}
