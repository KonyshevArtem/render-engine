#include "profiler.h"
#include "graphics_backend_api.h"
#include <vector>

constexpr int k_MaxFrames = 10;

int s_FrameCounter = 0;
bool s_IsEnabled = false;

int s_ContextDepth[static_cast<int>(Profiler::MarkerContext::MAX)];
std::vector<Profiler::FrameInfo> s_ContextFrames[static_cast<int>(Profiler::MarkerContext::MAX)];
std::vector<Profiler::GPUMarkerInfo> s_PendingGPUMarkers;

int IncrementDepth(Profiler::MarkerContext context)
{
    return s_ContextDepth[static_cast<int>(context)]++;
}

int DecrementDepth(Profiler::MarkerContext context)
{
    return s_ContextDepth[static_cast<int>(context)]--;
}

Profiler::MarkerInfo::MarkerInfo(MarkerType type, const char* name, int depth) :
    Type(type),
    Begin(std::chrono::system_clock::now()),
    End(Begin),
    Name(name),
    Depth(depth)
{
}

Profiler::GPUMarkerInfo::GPUMarkerInfo(const char* name, int depth, int frameNumber) :
    ProfilerMarker(GraphicsBackend::Current()->PushProfilerMarker()),
    Name(name),
    Depth(depth),
    FrameNumber(frameNumber)
{
}

Profiler::GPUMarkerInfo& Profiler::GPUMarkerInfo::operator=(const GPUMarkerInfo& info)
{
    ProfilerMarker = info.ProfilerMarker;
    Name = info.Name;
    Depth = info.Depth;
    FrameNumber = info.FrameNumber;
    return *this;
}

Profiler::Marker::Marker(const char* name) :
    m_Context(MarkerContext::MAIN_THREAD),
    m_Info(MarkerType::MARKER, name, IncrementDepth(m_Context))
{
}

Profiler::Marker::~Marker()
{
    m_Info.End = std::chrono::system_clock::now();
    AddMarkerInfo(m_Context, m_Info);
    DecrementDepth(m_Context);
}

Profiler::GPUMarker::GPUMarker(const char* name) :
    m_Context(MarkerContext::GPU_RENDER),
    m_Info(name, IncrementDepth(m_Context), s_FrameCounter)
{
}

Profiler::GPUMarker::~GPUMarker()
{
    GraphicsBackend::Current()->PopProfilerMarker(m_Info.ProfilerMarker);
    if (s_IsEnabled)
        s_PendingGPUMarkers.push_back(m_Info);
    DecrementDepth(m_Context);
}

void Profiler::SetEnabled(bool enabled)
{
    s_IsEnabled = enabled;
}

void Profiler::BeginNewFrame()
{
    ++s_FrameCounter;

    if (!s_IsEnabled)
        return;

    for (std::vector<FrameInfo>& contextFrames : s_ContextFrames)
    {
        if (contextFrames.size() >= k_MaxFrames)
            contextFrames.erase(contextFrames.begin());

        FrameInfo& newFrame = contextFrames.emplace_back();
        newFrame.FrameNumber = s_FrameCounter;
    }

    FrameInfo& newMainThreadFrame = s_ContextFrames[static_cast<int>(MarkerContext::MAIN_THREAD)].back();
    newMainThreadFrame.Markers.emplace_back(MarkerType::SEPARATOR);

    for (int i = 0; i < s_PendingGPUMarkers.size(); ++i)
    {
        const GPUMarkerInfo& gpuMarker = s_PendingGPUMarkers[i];

        uint64_t startTime;
        uint64_t endTime;
        if (GraphicsBackend::Current()->ResolveProfilerMarker(gpuMarker.ProfilerMarker, startTime, endTime))
        {
            MarkerInfo markerInfo(MarkerType::MARKER, gpuMarker.Name, gpuMarker.Depth);
            markerInfo.Begin = std::chrono::system_clock::time_point(std::chrono::microseconds(startTime));
            markerInfo.End = std::chrono::system_clock::time_point(std::chrono::microseconds(endTime));
            AddMarkerInfo(MarkerContext::GPU_RENDER, markerInfo, gpuMarker.FrameNumber);

            s_PendingGPUMarkers[i] = s_PendingGPUMarkers.back();
            s_PendingGPUMarkers.pop_back();
            --i;
        }
    }

    std::vector<FrameInfo>& gpuFrames = s_ContextFrames[static_cast<int>(MarkerContext::GPU_RENDER)];
    for (FrameInfo& gpuFrame: gpuFrames)
    {
        if (!gpuFrame.IsSorted && gpuFrame.Markers.size() > 1)
        {
            std::ranges::sort(gpuFrame.Markers, [](const MarkerInfo& info1, const MarkerInfo& info2){ return info1.Begin < info2.Begin; });
            gpuFrame.IsSorted = true;
        }
    }
}

void Profiler::AddMarkerInfo(MarkerContext context, const MarkerInfo& markerInfo, int frame)
{
    if (!s_IsEnabled)
        return;

    std::vector<FrameInfo>& contextFrames = s_ContextFrames[static_cast<int>(context)];

    FrameInfo* frameInfo = nullptr;
    if (frame == -1)
        frameInfo = &contextFrames.back();
    else
    {
        for (FrameInfo& info : contextFrames)
        {
            if (info.FrameNumber == frame)
            {
                frameInfo = &info;
                break;
            }
        }
    }

    if (frameInfo)
    {
        frameInfo->MaxDepth = std::max(frameInfo->MaxDepth, markerInfo.Depth);
        frameInfo->Markers.push_back(markerInfo);
        frameInfo->IsSorted = false;
    }
}

std::vector<Profiler::FrameInfo>& Profiler::GetContextFrames(MarkerContext context)
{
    return s_ContextFrames[static_cast<int>(context)];
}
