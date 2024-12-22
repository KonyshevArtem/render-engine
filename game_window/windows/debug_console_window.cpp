#ifdef ENABLE_IMGUI

#include "debug_console_window.h"
#include "imgui.h"
#include "debug.h"

#include <utility>

int s_WindowsCount = 0;
ImVec4 s_LogHoverColor = {0.12f, 0.12f, 0.12f, 1};
ImVec4 s_LogSelectedColor = {0.15f, 0.15f, 0.15f, 1};
ImVec4 s_ErrorLogTextColor = {1, 0.1f, 0.1f, 1};

DebugConsoleWindow::DebugConsoleWindow() : BaseWindow(600, 800, "Debug Console", typeid(DebugConsoleWindow).hash_code())
{
    ++s_WindowsCount;
    Debug::AddListener(m_Id, [this](std::string string) -> void { OnDebugLog(std::move(string)); });
}

DebugConsoleWindow::~DebugConsoleWindow()
{
    --s_WindowsCount;
    Debug::RemoveListener(m_Id);
}

void DrawLog(const std::string &log, int index, int &selectedIndex)
{
    bool isSelected = index == selectedIndex;
    ImGui::PushStyleColor(ImGuiCol_Header, s_LogSelectedColor);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, s_LogSelectedColor);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, isSelected ? s_LogSelectedColor : s_LogHoverColor);
    ImGui::PushStyleColor(ImGuiCol_Text, s_ErrorLogTextColor);

    std::string firstLine = log.substr(0, log.find('\n'));
    std::string elementId = "##" + std::to_string(index + 1); // append element id, so several elements with the same log are possible
    if (ImGui::Selectable((firstLine + elementId).c_str(), selectedIndex == index))
    {
        selectedIndex = index;
    }
    ImGui::PopStyleColor(4);
    ImGui::Separator();
}

void DebugConsoleWindow::DrawTopBar()
{
    if (ImGui::Button(m_Paused ? "Resume" : "Pause"))
    {
        m_Paused = !m_Paused;
    }

    if (ImGui::Button("Clear"))
    {
        m_Logs.clear();
    }
}

void DebugConsoleWindow::DrawInternal()
{
    if (ImGui::BeginChild("Small Logs", {0, -200}, ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY))
    {
        ImGuiListClipper clipper;
        clipper.Begin(m_Logs.size());
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            {
                DrawLog(m_Logs[i], i, m_Selected);
            }
        }

        if (m_NewLogAdded)
        {
            ImGui::SetScrollY(ImGui::GetScrollMaxY() + ImGui::GetItemRectMax().y);
        }
    }
    ImGui::EndChild();

    if (ImGui::BeginChild("Full Log", ImVec2(0, 0), ImGuiChildFlags_Border))
    {
        if (m_Selected != -1)
        {
            ImGui::TextWrapped(m_Logs[m_Selected].c_str());
        }
    }
    ImGui::EndChild();

    m_NewLogAdded = false;
}

void DebugConsoleWindow::OnDebugLog(std::string string)
{
    if (!m_Paused)
    {
        m_Logs.push_back(std::move(string));
        m_NewLogAdded = true;
    }
}

void CreateWindowListener(std::string _)
{
    if (s_WindowsCount == 0)
    {
        WindowManager::Create<DebugConsoleWindow>();
    }
}

DEFINE_STATIC_CONSTRUCTOR(DebugConsoleWindow)
{
    Debug::AddListener(-1, CreateWindowListener);
}

#endif
