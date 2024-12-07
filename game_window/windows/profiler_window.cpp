#ifdef ENABLE_IMGUI

#include "profiler_window.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "editor/profiler/profiler.h"

#include <typeinfo>

constexpr int k_DefaultRangeMicroseconds = 500000;
constexpr int k_MinRangeMicroseconds = 16000;
constexpr int k_ZoomSpeedMicroseconds = 16000;

int64_t GetMicroseconds(const std::chrono::high_resolution_clock::time_point& begin, const std::chrono::high_resolution_clock::time_point& end)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
}

void DrawFrameSeparator(double separatorX)
{
    const ImVec2 windowPos = ImGui::GetWindowPos();
    const float linePosX = separatorX + windowPos.x;
    const float lineStartPosY = ImGui::GetWindowContentRegionMin().y + windowPos.y;
    const float lineEndPosY = ImGui::GetWindowContentRegionMax().y + windowPos.y;
    ImGui::GetWindowDrawList()->AddLine({linePosX, lineStartPosY}, {linePosX, lineEndPosY}, IM_COL32(255, 0, 0, 255), 1);
}

ProfilerWindow::ProfilerWindow() :
    BaseWindow(1400, 800, "Hierarchy", typeid(ProfilerWindow).hash_code(), false),
    m_CurrentRange(k_DefaultRangeMicroseconds),
    m_Offset(0)
{
    Profiler::SetEnabled(true);
}

ProfilerWindow::~ProfilerWindow()
{
    Profiler::SetEnabled(false);
}

void ProfilerWindow::DrawInternal()
{
    const std::vector<std::vector<Profiler::MarkerInfo>>& frameMarkerInfos = Profiler::GetMarkerInfos();
    if (frameMarkerInfos.empty())
        return;

    HandleZoom();

    const std::chrono::high_resolution_clock::time_point rangeEnd = std::chrono::high_resolution_clock::now() - m_Offset;
    const std::chrono::high_resolution_clock::time_point rangeBegin = rangeEnd - m_CurrentRange;

    const double rangeToWidth = ImGui::GetWindowContentRegionMax().x / static_cast<double>(m_CurrentRange.count());

    ImGui::BeginChild("Content");
    HandleDrag(rangeToWidth);

    for (const std::vector<Profiler::MarkerInfo>& markerInfos : frameMarkerInfos)
    {
        DrawFrameSeparator(GetMicroseconds(rangeBegin, markerInfos[0].Begin) * rangeToWidth);

        for (const Profiler::MarkerInfo& marker : markerInfos)
        {
            const double duration = GetMicroseconds(marker.Begin, marker.End);
            const float posX = GetMicroseconds(rangeBegin, marker.Begin) * rangeToWidth;
            const float posY = ImGui::GetWindowContentRegionMin().y;
            const float width = duration * rangeToWidth;

            ImGui::SetCursorPos({posX, posY});
            ImGui::Button(marker.Name, {width, 0});
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(std::format("{}\n{}ms", marker.Name, duration / 1000.0).c_str());
            }
        }
    }

    ImGui::EndChild();
}

void ProfilerWindow::HandleZoom()
{
    const float mouseWheelInput = ImGui::GetIO().MouseWheel;
    if (mouseWheelInput != 0)
    {
        m_CurrentRange -= std::chrono::microseconds(static_cast<int>(mouseWheelInput * k_ZoomSpeedMicroseconds));
        if (m_CurrentRange.count() < k_MinRangeMicroseconds)
            m_CurrentRange = std::chrono::microseconds(k_MinRangeMicroseconds);
    }
}

void ProfilerWindow::HandleDrag(double rangeToWidth)
{
    if (ImGui::IsItemActive())
    {
        const int mouseDeltaX = ImGui::GetIO().MouseDelta.x / rangeToWidth;
        if (mouseDeltaX != 0)
        {
            m_Offset += std::chrono::microseconds(mouseDeltaX);
            if (m_Offset.count() < 0)
                m_Offset = std::chrono::microseconds(0);
        }
    }
}

#endif
