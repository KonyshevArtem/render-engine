#include "engine_framework.h"
#include "graphics_backend_api.h"
#include "file_system/file_system.h"

#include <GLFW/glfw3.h>
#include <windows.h>

GLFWwindow *s_Window = nullptr;

struct ImGuiInitData
{
    void* Window;
    int OpenGLMajorVersion;
    int OpenGLMinorVersion;
};

void KeyboardFunction(GLFWwindow *window, int keycode, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        EngineFramework::ProcessKeyPress(static_cast<char>(keycode), action == GLFW_PRESS);
    }
}

void MouseClickFunction(GLFWwindow* window, int button, int action, int mods)
{
    EngineFramework::ProcessMouseClick(button, action == GLFW_PRESS);
}

void MouseMoveFunction(GLFWwindow *window, double x, double y)
{
    EngineFramework::ProcessMouseMove(static_cast<float>(x), static_cast<float>(y));
}

int main(int argc, char **argv)
{
    char executablePath[MAX_PATH];
    GetModuleFileNameA(NULL, executablePath, MAX_PATH);
    std::string resourcesPath = std::filesystem::path(executablePath).parent_path().string();

    FileSystem::FileSystemData fileSystemData{};
    fileSystemData.ExecutablePath = executablePath;
    fileSystemData.ResourcesPath = resourcesPath.c_str();

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

    glfwSetMouseButtonCallback(s_Window, MouseClickFunction);
    glfwSetCursorPosCallback(s_Window, MouseMoveFunction);
    glfwSetKeyCallback(s_Window, KeyboardFunction);

    ImGuiInitData imGuiInitData = ImGuiInitData();
    imGuiInitData.Window = s_Window;
    imGuiInitData.OpenGLMajorVersion = OPENGL_MAJOR_VERSION;
    imGuiInitData.OpenGLMinorVersion = OPENGL_MINOR_VERSION;

    EngineFramework::Initialize(static_cast<void*>(&fileSystemData), nullptr, static_cast<void*>(&imGuiInitData), "OpenGL");

    while (!glfwWindowShouldClose(s_Window))
    {
        if (EngineFramework::ShouldCloseWindow())
        {
            break;
        }

        glfwPollEvents();
        glfwGetFramebufferSize(s_Window, &width, &height);

        EngineFramework::TickMainLoop(nullptr, nullptr, width, height);

        glfwSwapBuffers(s_Window);
    }

    EngineFramework::Shutdown();

    // Don't call those to keep OpenGL context, so objects can be destroyed
    // TODO: refactor when resources are properly managed
    //glfwDestroyWindow(s_Window);
    //glfwTerminate();

    return 0;
}
