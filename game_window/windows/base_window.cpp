#include "base_window.h"
#include "windows_id_pool.h"
#include "imgui.h"

#include <vector>

WindowsIdPool s_WindowsIdPool;
std::vector<std::shared_ptr<BaseWindow>> s_Windows;

BaseWindow::BaseWindow(float width, float height, const std::string &title, size_t typeHashCode) :
        m_TypeHashCode(typeHashCode),
        m_Id(s_WindowsIdPool.GetID(typeHashCode)),
        m_Title(title + "##" + std::to_string(m_Id)),
        m_IsOpened(true),
        m_InitialWidth(width),
        m_InitialHeight(height)
{
}

void BaseWindow::DrawAllWindows()
{
    for (auto it = s_Windows.begin(); it != s_Windows.end();)
    {
        auto window = it->get();
        window->Draw();

        if (window->m_IsOpened)
        {
            ++it;
        }
        else
        {
            s_WindowsIdPool.ReturnID(window->m_TypeHashCode, window->m_Id);
            it = s_Windows.erase(it);
        }
    }
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

void BaseWindow::AddWindow(const std::shared_ptr<BaseWindow> &window)
{
    s_Windows.push_back(window);
}
