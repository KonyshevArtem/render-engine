#include "profiler.h"
#include "graphics_backend_api.h"

#include <vector>
#include <algorithm>

constexpr int k_MaxFrames = 10;

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

void SortMarkers(Profiler::MarkerContext context)
{
    std::vector<Profiler::FrameInfo>& gpuFrames = s_ContextFrames[static_cast<int>(context)];
    for (Profiler::FrameInfo& gpuFrame: gpuFrames)
    {
        if (!gpuFrame.IsSorted && gpuFrame.Markers.size() > 1)
        {
            std::ranges::sort(gpuFrame.Markers, [](const Profiler::MarkerInfo& info1, const Profiler::MarkerInfo& info2){ return info1.Begin < info2.Begin; });
            gpuFrame.IsSorted = true;
        }
    }
}

Profiler::MarkerInfo::MarkerInfo(MarkerType type, const char* name, int depth, uint64_t frame) :
    Type(type),
    Begin(std::chrono::system_clock::now()),
    End(Begin),
    Name(name),
    Depth(depth),
    Frame(frame)
{
}

Profiler::GPUMarkerInfo::GPUMarkerInfo(const char* name, int depth, uint64_t frame) :
    ProfilerMarker(),
    Name(name),
    Depth(depth),
    Frame(frame)
{
    if (s_IsEnabled)
        ProfilerMarker = GraphicsBackend::Current()->PushProfilerMarker();
}

Profiler::GPUMarkerInfo& Profiler::GPUMarkerInfo::operator=(const GPUMarkerInfo& info)
{
    ProfilerMarker = info.ProfilerMarker;
    Name = info.Name;
    Depth = info.Depth;
    Frame = info.Frame;
    return *this;
}

Profiler::Marker::Marker(const char* name) :
    m_Context(MarkerContext::MAIN_THREAD),
    m_Info(MarkerType::MARKER, name, IncrementDepth(m_Context), GraphicsBackend::Current()->GetFrameNumber())
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
    m_Info(name, IncrementDepth(m_Context), GraphicsBackend::Current()->GetFrameNumber())
{
}

Profiler::GPUMarker::~GPUMarker()
{
    if (s_IsEnabled)
    {
        GraphicsBackend::Current()->PopProfilerMarker(m_Info.ProfilerMarker);
        s_PendingGPUMarkers.push_back(m_Info);
    }
    DecrementDepth(m_Context);
}

void Profiler::SetEnabled(bool enabled)
{
    s_IsEnabled = enabled;
}

void Profiler::BeginNewFrame()
{
    if (!s_IsEnabled)
        return;

    for (std::vector<FrameInfo>& contextFrames : s_ContextFrames)
    {
        if (contextFrames.size() >= k_MaxFrames)
            contextFrames.erase(contextFrames.begin());

        FrameInfo& newFrame = contextFrames.emplace_back();
        newFrame.Frame = GraphicsBackend::Current()->GetFrameNumber();
    }

    FrameInfo& newMainThreadFrame = s_ContextFrames[static_cast<int>(MarkerContext::MAIN_THREAD)].back();
    newMainThreadFrame.Markers.emplace_back(MarkerType::SEPARATOR);

    for (int i = 0; i < s_PendingGPUMarkers.size(); ++i)
    {
        const GPUMarkerInfo& gpuMarker = s_PendingGPUMarkers[i];

        ProfilerMarkerResolveResults results;
        if (GraphicsBackend::Current()->ResolveProfilerMarker(gpuMarker.ProfilerMarker, results))
        {
            for (int gpuQueue = 0; gpuQueue < static_cast<int>(GPUQueue::MAX); ++gpuQueue)
            {
                auto queueToContext = [](int queueIndex)
                {
                    const GPUQueue queue = static_cast<GPUQueue>(queueIndex);
                    switch (queue)
                    {
                        case GPUQueue::COPY:
                            return MarkerContext::GPU_COPY;
                        default:
                            return MarkerContext::GPU_RENDER;
                    }
                };

                const ProfilerMarkerResolveResult& result = results[gpuQueue];
                if (!result.IsActive)
                    continue;

                MarkerInfo markerInfo(MarkerType::MARKER, gpuMarker.Name, gpuMarker.Depth, gpuMarker.Frame);
                markerInfo.Begin = std::chrono::system_clock::time_point(std::chrono::microseconds(result.StartTimestamp));
                markerInfo.End = std::chrono::system_clock::time_point(std::chrono::microseconds(result.EndTimestamp));
                AddMarkerInfo(queueToContext(gpuQueue), markerInfo, gpuMarker.Frame);
            }

            s_PendingGPUMarkers[i] = s_PendingGPUMarkers.back();
            s_PendingGPUMarkers.pop_back();
            --i;
        }
    }

    SortMarkers(MarkerContext::GPU_RENDER);
    SortMarkers(MarkerContext::GPU_COPY);
}

void Profiler::AddMarkerInfo(MarkerContext context, const MarkerInfo& markerInfo, uint64_t frame)
{
    if (!s_IsEnabled)
        return;

    std::vector<FrameInfo>& contextFrames = s_ContextFrames[static_cast<int>(context)];

    FrameInfo* frameInfo = nullptr;
    if (frame == 0)
    {
        if (!contextFrames.empty())
            frameInfo = &contextFrames.back();
    }
    else
    {
        for (FrameInfo& info : contextFrames)
        {
            if (info.Frame == frame)
            {
                frameInfo = &info;
                break;
            }
        }
    }

    if (frameInfo)
    {
        frameInfo->Markers.push_back(markerInfo);
        frameInfo->IsSorted = false;
    }
}

std::vector<Profiler::FrameInfo>& Profiler::GetContextFrames(MarkerContext context)
{
    return s_ContextFrames[static_cast<int>(context)];
}
