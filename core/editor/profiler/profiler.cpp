#include "profiler.h"
#include <vector>

constexpr int k_MaxFrames = 10;

int s_Depth = 0;
bool s_IsEnabled = false;
std::vector<std::vector<Profiler::MarkerInfo>> s_MarkerInfos;

Profiler::Marker::Marker(const char* name) :
    Info{std::chrono::high_resolution_clock::now(), std::chrono::high_resolution_clock::now(), name, s_Depth++}
{
}

Profiler::Marker::~Marker()
{
    Info.End = std::chrono::high_resolution_clock::now();
    Profiler::AddMarkerInfo(Info);
    s_Depth--;
}

void Profiler::SetEnabled(bool enabled)
{
    s_IsEnabled = enabled;
}

void Profiler::BeginNewFrame()
{
    if (!s_IsEnabled)
        return;

    if (s_MarkerInfos.size() >= k_MaxFrames)
        s_MarkerInfos.erase(s_MarkerInfos.begin());

    s_MarkerInfos.emplace_back();
}

void Profiler::AddMarkerInfo(const MarkerInfo& markerInfo)
{
    if (s_IsEnabled && !s_MarkerInfos.empty())
        s_MarkerInfos.back().push_back(markerInfo);
}

std::vector<std::vector<Profiler::MarkerInfo>>& Profiler::GetMarkerInfos()
{
    return s_MarkerInfos;
}
