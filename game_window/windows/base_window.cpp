#include "base_window.h"
#include "windows_id_pool.h"
#include "imgui.h"

WindowsIdPool s_WindowsIdPool;

BaseWindow::BaseWindow(float width, float height, const std::string &title, size_t typeHashCode) :
        m_TypeHashCode(typeHashCode),
        m_Id(s_WindowsIdPool.GetID(typeHashCode)),
        m_Title(title + "##" + std::to_string(m_Id)),
        m_IsOpened(true),
        m_InitialWidth(width),
        m_InitialHeight(height)
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
        if (ImGui::Begin(m_Title.c_str(), &m_IsOpened, ImGuiWindowFlags_MenuBar))
        {
            DrawInternal();
        }

        ImGui::End();
    }
}
