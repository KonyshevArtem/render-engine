#include "game_window.h"
#include "graphics_backend_api.h"

#ifdef ENABLE_IMGUI
#include "top_menu_bar.h"
#include "window_manager.h"
#include "imgui.h"
#endif

#include <utility>
#include <string>

GameWindow::GameWindow(RenderHandler renderHandler):
    m_RenderHandler(std::move(renderHandler))
{
}

GameWindow::~GameWindow()
{
#ifdef ENABLE_IMGUI
    WindowManager::CloseAllWindows();
#endif
}

void GameWindow::TickMainLoop(int width, int height)
{
    if (m_RenderHandler)
    {
        m_RenderHandler(width, height);
    }

#ifdef ENABLE_IMGUI
    TopMenuBar::Draw();
    WindowManager::DrawAllWindows();

    const std::string stats = "Draw Calls: " + std::to_string(GraphicsBackend::Current()->GetDrawCallCount());
    const float statsHeight = ImGui::CalcTextSize(stats.c_str()).y;
    ImGui::GetForegroundDrawList()->AddText(ImVec2(0, ImGui::GetIO().DisplaySize.y - statsHeight), IM_COL32(255, 255, 255, 255), stats.c_str());
#endif
}

bool GameWindow::ShouldCloseWindow() const
{
    return m_CloseFlag;
}

bool GameWindow::CaptureKeyboard() const
{
#ifdef ENABLE_IMGUI
    return ImGui::GetIO().WantCaptureKeyboard;
#else
    return false;
#endif
}

bool GameWindow::CaptureMouse() const
{
#ifdef ENABLE_IMGUI
    return ImGui::GetIO().WantCaptureMouse;
#else
    return false;
#endif
}
