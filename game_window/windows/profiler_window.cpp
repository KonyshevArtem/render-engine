#ifdef ENABLE_IMGUI

#include "profiler_window.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "editor/profiler/profiler.h"

#include <typeinfo>
#include <vector>
#include <string>

constexpr int k_DefaultRangeMicroseconds = 33000;
constexpr int k_MinRangeMicroseconds = 1000;
constexpr int k_ZoomSpeedMicroseconds = 4000;

constexpr int k_MarkerContentMargin = 40;
constexpr int k_MarkerHeight = 20;

int64_t GetMicroseconds(const std::chrono::system_clock::time_point& begin, const std::chrono::system_clock::time_point& end)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
}

void DrawSeparator(const Profiler::MarkerInfo& marker, std::chrono::system_clock::time_point rangeBegin, double rangeToWidth)
{
    assert(marker.Type == Profiler::MarkerType::SEPARATOR);

    const ImVec2 windowPos = ImGui::GetWindowPos();
    const float linePosX = GetMicroseconds(rangeBegin, marker.Begin) * rangeToWidth + windowPos.x;
    const float lineStartPosY = ImGui::GetWindowContentRegionMin().y + windowPos.y;
    const float lineEndPosY = ImGui::GetWindowContentRegionMax().y + windowPos.y;
    ImGui::GetWindowDrawList()->AddLine({linePosX, lineStartPosY}, {linePosX, lineEndPosY}, IM_COL32(255, 0, 0, 255), 1);
}

void DrawMarker(const Profiler::MarkerInfo& marker, std::chrono::system_clock::time_point rangeBegin, double rangeToWidth, int depthOffset)
{
    assert(marker.Type == Profiler::MarkerType::MARKER);

    const double duration = std::max(GetMicroseconds(marker.Begin, marker.End), int64_t(1));
    const float posX = GetMicroseconds(rangeBegin, marker.Begin) * rangeToWidth;
    const float posY = ImGui::GetWindowContentRegionMin().y + (marker.Depth + depthOffset) * k_MarkerHeight;
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

ProfilerWindow::DraggableContentRegion::DraggableContentRegion(const char* id, double rangeToWidth, float width, float height, ProfilerWindow* profilerWindow) :
    m_ProfilerWindow(profilerWindow)
{
    ImGui::BeginChild(id, {width, height});
    m_ProfilerWindow->HandleDrag(rangeToWidth);
}

ProfilerWindow::DraggableContentRegion::~DraggableContentRegion()
{
    ImGui::EndChild();
    m_ProfilerWindow->HandleZoom(); // call after EndChild for IsItemHovered to work
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
    const std::vector<Profiler::FrameInfo>& mainThreadFrames = Profiler::GetContextFrames(Profiler::MarkerContext::MAIN_THREAD);
    if (mainThreadFrames.empty() || mainThreadFrames.back().Markers.empty())
        return;

    const std::chrono::system_clock::time_point rangeEnd = mainThreadFrames.back().Markers.back().End - m_Offset;
    const std::chrono::system_clock::time_point rangeBegin = rangeEnd - m_CurrentRange;

    const double rangeToWidth = ImGui::GetWindowContentRegionMax().x / static_cast<double>(m_CurrentRange.count());

    auto GetContextLabel = [](Profiler::MarkerContext context) -> const char*
    {
        switch (context)
        {
            case Profiler::MarkerContext::MAIN_THREAD:
                return "Main Thread";
            case Profiler::MarkerContext::GPU_RENDER:
                return "GPU Render";
            case Profiler::MarkerContext::GPU_COPY:
                return "GPU Copy";
        }
    };

    DraggableContentRegion region("Content", rangeToWidth, 0, 0, this);
    {
        for (int i = 0; i < static_cast<int>(Profiler::MarkerContext::MAX); ++i)
        {
            const Profiler::MarkerContext context = static_cast<Profiler::MarkerContext>(i);
            DrawMarkers(GetContextLabel(context), context, rangeBegin, rangeToWidth);
        }
    }
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

void ProfilerWindow::DrawMarkers(const std::string& label, Profiler::MarkerContext context, std::chrono::system_clock::time_point rangeBegin, double rangeToWidth)
{
    static int maxDepths[static_cast<int>(Profiler::MarkerContext::MAX)];

    const std::vector<Profiler::FrameInfo>& profilerFrames = Profiler::GetContextFrames(context);
    if (profilerFrames.empty())
        return;

    ImGui::Text(label.c_str());

    int& maxDepth = maxDepths[static_cast<int>(context)];
    const int markerLines = maxDepth + 1;
    const bool handleOverlap = context == Profiler::MarkerContext::GPU_RENDER || context ==  Profiler::MarkerContext::GPU_COPY;

    std::vector<uint64_t> markerLinesMaxTimestamp;
    if (handleOverlap)
    {
        markerLinesMaxTimestamp.reserve(markerLines);
        for (int i = 0; i < markerLines; ++i)
            markerLinesMaxTimestamp.push_back(0);
    }

    DraggableContentRegion region(label.c_str(), rangeToWidth, 0, markerLines * k_MarkerHeight + k_MarkerContentMargin, this);
    {
        for (const Profiler::FrameInfo& frameInfo : profilerFrames)
        {
            for (const Profiler::MarkerInfo& marker : frameInfo.Markers)
            {
                int depthOffset = 0;
                if (handleOverlap)
                {
                    bool depthFound = false;
                    const uint64_t beginTimestamp = marker.Begin.time_since_epoch().count();
                    for (int i = 0; i < markerLinesMaxTimestamp.size(); ++i)
                    {
                        const int currentDepth = (marker.Depth + i) % markerLinesMaxTimestamp.size();
                        const uint64_t maxTimestampAtDepth = markerLinesMaxTimestamp[currentDepth];
                        if (beginTimestamp >= maxTimestampAtDepth)
                        {
                            depthFound = true;
                            depthOffset = currentDepth - marker.Depth;
                            break;
                        }
                    }

                    if (!depthFound)
                    {
                        markerLinesMaxTimestamp.push_back(0);
                        depthOffset = markerLinesMaxTimestamp.size() - marker.Depth - 1;
                    }

                    markerLinesMaxTimestamp[marker.Depth + depthOffset] = marker.End.time_since_epoch().count();
                }

                maxDepth = std::max(maxDepth, marker.Depth + depthOffset);

                switch (marker.Type)
                {
                    case Profiler::MarkerType::MARKER:
                        DrawMarker(marker, rangeBegin, rangeToWidth, depthOffset);
                        break;
                    case Profiler::MarkerType::SEPARATOR:
                        DrawSeparator(marker, rangeBegin, rangeToWidth);
                        break;
                }
            }
        }
    }
}

#endif
