#include "game_window.h"
#include "graphics_backend_api.h"
#include "top_menu_bar.h"
#include "window_manager.h"

#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"

#include <utility>
#if __has_include("GLUT/glut.h")
#include <GLUT/glut.h>
#elif __has_include("GL/freeglut.h")
#include <GL/freeglut.h>
#endif

ResizeHandler s_ResizeHandler;
RenderHandler s_RenderHandler;
KeyboardInputHandlerDelegate s_KeyboardInputHandler;
MouseMoveHandlerDelegate s_MouseMoveHandler;

void ResizeFunction(int width, int height);
void RenderFunction();
void KeyboardPressFunction(unsigned char key, int x, int y);
void KeyboardReleaseFunction(unsigned char key, int x, int y);
void MouseMoveFunction(int x, int y);

GameWindow::GameWindow(int width,
                       int height,
                       ResizeHandler resizeHandler,
                       RenderHandler renderHandler,
                       KeyboardInputHandlerDelegate keyboardInputHandler,
                       MouseMoveHandlerDelegate mouseMoveHandler)
{
    s_ResizeHandler = std::move(resizeHandler);
    s_RenderHandler = std::move(renderHandler);
    s_KeyboardInputHandler = std::move(keyboardInputHandler);
    s_MouseMoveHandler = std::move(mouseMoveHandler);

    unsigned int displayMode = GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH;
#ifdef GLUT_3_2_CORE_PROFILE
    displayMode |= GLUT_3_2_CORE_PROFILE;
#endif

    int argc = 0;
    glutInit(&argc, nullptr);
    glutInitDisplayMode(displayMode);
    glutInitWindowSize(width, height);
#ifdef GLUT_CORE_PROFILE
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitContextVersion(GraphicsBackend::GetMajorVersion(), GraphicsBackend::GetMinorVersion());
#endif
    glutCreateWindow("GameWindow");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init(GraphicsBackend::GetShadingLanguageDirective().c_str());
    ImGui_ImplGLUT_InstallFuncs();

    // call these after installing handles for ImGui to override them
    // ImGui handles are called inside of custom handlers
    glutKeyboardFunc(KeyboardPressFunction);
    glutKeyboardUpFunc(KeyboardReleaseFunction);
    glutMotionFunc(MouseMoveFunction);
    glutPassiveMotionFunc(MouseMoveFunction);

    glutDisplayFunc(RenderFunction);
    glutReshapeFunc(ResizeFunction);
}

GameWindow::~GameWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}

void GameWindow::BeginMainLoop()
{
    glutMainLoop();
}

void ResizeFunction(int width, int height)
{
    ImGui_ImplGLUT_ReshapeFunc(width, height);

    if (s_ResizeHandler)
    {
        s_ResizeHandler(width, height);
    }
}

void RenderFunction()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    if (s_RenderHandler)
    {
        s_RenderHandler();
    }

    TopMenuBar::Draw();
    WindowManager::DrawAllWindows();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glutSwapBuffers();
    glutPostRedisplay();
}

void KeyboardPressFunction(unsigned char key, int x, int y)
{
    auto &io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
    {
        ImGui_ImplGLUT_KeyboardFunc(key, x, y);
    }
    else if (s_KeyboardInputHandler)
    {
        s_KeyboardInputHandler(static_cast<char>(key), true);
    }
}

void KeyboardReleaseFunction(unsigned char key, int x, int y)
{
    auto &io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
    {
        ImGui_ImplGLUT_KeyboardUpFunc(key, x, y);
    }
    else if (s_KeyboardInputHandler)
    {
        s_KeyboardInputHandler(static_cast<char>(key), false);
    }
}

void MouseMoveFunction(int x, int y)
{
    auto &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        ImGui_ImplGLUT_MotionFunc(x, y);
    }
    else if (s_MouseMoveHandler)
    {
        s_MouseMoveHandler(x, y);
    }
}