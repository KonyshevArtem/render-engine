#ifdef ENABLE_IMGUI

#include "base_window.h"
#include "windows_id_pool.h"
#include "imgui.h"

WindowsIdPool s_WindowsIdPool;

BaseWindow::BaseWindow(float width, float height, const std::string &title, size_t typeHashCode, bool topBar) :
        m_TypeHashCode(typeHashCode),
        m_Id(s_WindowsIdPool.GetID(typeHashCode)),
        m_Title(title + "##" + std::to_string(m_Id)),
        m_IsOpened(true),
        m_InitialWidth(width),
        m_InitialHeight(height),
        m_TopBar(topBar)
{
}

BaseWindow::~BaseWindow()
{
    s_WindowsIdPool.ReturnID(m_TypeHashCode, m_Id);
}

void BaseWindow::Draw()
{
    if (m_IsOpened)
    {
        ImGui::SetNextWindowSize({m_InitialWidth, m_InitialHeight}, ImGuiCond_FirstUseEver);

        ImGuiWindowFlags_ flags = m_TopBar ? ImGuiWindowFlags_MenuBar : ImGuiWindowFlags_None;
        if (ImGui::Begin(m_Title.c_str(), &m_IsOpened, flags))
        {
            if (m_TopBar && ImGui::BeginMenuBar())
            {
                DrawTopBar();
                ImGui::EndMenuBar();
            }

            DrawInternal();
        }

        ImGui::End();
    }
}

#endif