#ifdef ENABLE_IMGUI

#include "profiler_window.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "editor/profiler/profiler.h"

#include <typeinfo>
#include <vector>

constexpr int k_DefaultRangeMicroseconds = 33000;
constexpr int k_MinRangeMicroseconds = 1000;
constexpr int k_ZoomSpeedMicroseconds = 4000;
constexpr int k_MarkerHeight = 20;

int64_t GetMicroseconds(const std::chrono::high_resolution_clock::time_point& begin, const std::chrono::high_resolution_clock::time_point& end)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
}

void DrawSeparator(const Profiler::MarkerInfo& marker, std::chrono::high_resolution_clock::time_point rangeBegin, double rangeToWidth)
{
    assert(marker.Type == Profiler::MarkerType::SEPARATOR);

    const ImVec2 windowPos = ImGui::GetWindowPos();
    const float linePosX = GetMicroseconds(rangeBegin, marker.Begin) * rangeToWidth + windowPos.x;
    const float lineStartPosY = ImGui::GetWindowContentRegionMin().y + windowPos.y;
    const float lineEndPosY = ImGui::GetWindowContentRegionMax().y + windowPos.y;
    ImGui::GetWindowDrawList()->AddLine({linePosX, lineStartPosY}, {linePosX, lineEndPosY}, IM_COL32(255, 0, 0, 255), 1);
}

void DrawMarker(const Profiler::MarkerInfo& marker, std::chrono::high_resolution_clock::time_point rangeBegin, double rangeToWidth)
{
    assert(marker.Type == Profiler::MarkerType::MARKER);

    const double duration = GetMicroseconds(marker.Begin, marker.End);
    const float posX = GetMicroseconds(rangeBegin, marker.Begin) * rangeToWidth;
    const float posY = ImGui::GetWindowContentRegionMin().y + marker.Depth * k_MarkerHeight;
    const float width = duration * rangeToWidth;

    size_t nameHash = std::hash<std::string>{}(marker.Name);
    ImVec4 color = ImGui::ColorConvertU32ToFloat4(*reinterpret_cast<ImU32*>(&nameHash));
    color.x *= 0.5;
    color.y *= 0.5;
    color.z *= 0.5;
    color.w = 1;

    ImGui::SetCursorPos({posX, posY});
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    ImGui::Button(marker.Name, {width, k_MarkerHeight});
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(std::format("{}\n{}ms", marker.Name, duration / 1000.0).c_str());
    }
    ImGui::PopStyleColor(3);
}

ProfilerWindow::ProfilerWindow() :
    BaseWindow(1400, 800, "Profiler", typeid(ProfilerWindow).hash_code()),
    m_CurrentRange(k_DefaultRangeMicroseconds),
    m_Offset(0),
    m_IsEnabled(true)
{
    Profiler::SetEnabled(m_IsEnabled);
}

ProfilerWindow::~ProfilerWindow()
{
    Profiler::SetEnabled(false);
}

void ProfilerWindow::DrawTopBar()
{
    const char* buttonLabel = m_IsEnabled ? "Pause" : "Resume";
    if (ImGui::Button(buttonLabel))
    {
        m_IsEnabled = !m_IsEnabled;
        Profiler::SetEnabled(m_IsEnabled);
    }
}

void ProfilerWindow::DrawInternal()
{
    const std::vector<std::vector<Profiler::MarkerInfo>>& frameMarkerInfos = Profiler::GetMarkerInfos();
    if (frameMarkerInfos.empty())
        return;

    const std::chrono::high_resolution_clock::time_point rangeEnd = frameMarkerInfos.back().back().End - m_Offset;
    const std::chrono::high_resolution_clock::time_point rangeBegin = rangeEnd - m_CurrentRange;

    const double rangeToWidth = ImGui::GetWindowContentRegionMax().x / static_cast<double>(m_CurrentRange.count());

    ImGui::BeginChild("Content", {0, 0});
    HandleDrag(rangeToWidth);

    for (const std::vector<Profiler::MarkerInfo>& markerInfos : frameMarkerInfos)
    {
        for (const Profiler::MarkerInfo& marker : markerInfos)
        {
            switch (marker.Type)
            {
                case Profiler::MarkerType::MARKER:
                    DrawMarker(marker, rangeBegin, rangeToWidth);
                    break;
                case Profiler::MarkerType::SEPARATOR:
                    DrawSeparator(marker, rangeBegin, rangeToWidth);
                    break;
            }
        }
    }

    ImGui::EndChild();
    HandleZoom(); // call after EndChild for IsItemHovered to work
}

void ProfilerWindow::HandleZoom()
{
    if (ImGui::IsItemHovered())
    {
        const float mouseWheelInput = ImGui::GetIO().MouseWheel;
        if (mouseWheelInput != 0)
        {
            int zoomMicroseconds = static_cast<int>(mouseWheelInput * k_ZoomSpeedMicroseconds);
            m_CurrentRange -= std::chrono::microseconds(zoomMicroseconds);
            if (m_CurrentRange.count() < k_MinRangeMicroseconds)
            {
                m_CurrentRange = std::chrono::microseconds(k_MinRangeMicroseconds);
                zoomMicroseconds = 0;
            }

            const float mousePosX = ImGui::GetIO().MousePos.x - ImGui::GetWindowPos().x;
            const float mousePosNormalizedX = 1 - mousePosX / ImGui::GetWindowContentRegionMax().x;
            AddOffset(static_cast<int>(mousePosNormalizedX * zoomMicroseconds));
        }
    }
}

void ProfilerWindow::HandleDrag(double rangeToWidth)
{
    if (ImGui::IsItemActive())
    {
        const int mouseDeltaX = ImGui::GetIO().MouseDelta.x / rangeToWidth;
        if (mouseDeltaX != 0)
            AddOffset(mouseDeltaX);
    }
}

void ProfilerWindow::AddOffset(int offset)
{
    m_Offset += std::chrono::microseconds(offset);
    if (m_Offset.count() < 0)
        m_Offset = std::chrono::microseconds(0);
}

#endif
