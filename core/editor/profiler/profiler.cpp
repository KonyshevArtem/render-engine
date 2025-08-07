#include "profiler.h"
#include "graphics_backend_api.h"
#include "worker/worker.h"

#include <vector>
#include <algorithm>

constexpr int k_MaxFrames = 1000;
constexpr int k_ContextCount = static_cast<int>(Profiler::MarkerContext::MAX);

bool s_IsEnabled = false;

int s_ContextDepth[k_ContextCount];
std::mutex s_ContextMutexes[k_ContextCount];
std::map<uint64_t, Profiler::FrameInfo> s_ContextFrames[k_ContextCount];
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
    std::map<uint64_t, Profiler::FrameInfo>& gpuFrames = s_ContextFrames[static_cast<int>(context)];
    for (auto& pair: gpuFrames)
    {
        Profiler::FrameInfo& gpuFrame = pair.second;
        if (!gpuFrame.IsSorted && gpuFrame.Markers.size() > 1)
        {
            std::ranges::sort(gpuFrame.Markers, [](const Profiler::MarkerInfo& info1, const Profiler::MarkerInfo& info2){ return info1.Begin < info2.Begin; });
            gpuFrame.IsSorted = true;
        }
    }
}

Profiler::MarkerContext GetCPUContext()
{
    uint32_t workerId = Worker::GetWorkerId();
    return workerId == -1 ? Profiler::MarkerContext::MAIN_THREAD : static_cast<Profiler::MarkerContext>(static_cast<uint32_t>(Profiler::MarkerContext::WORKER_1) + workerId);
}

Profiler::MarkerInfo::MarkerInfo(MarkerType type, const char* name, std::optional<std::string> additionalInfo, int depth, uint64_t frame) :
    Type(type),
    Begin(std::chrono::system_clock::now()),
    End(Begin),
    Name(name),
    AdditionalInfo(std::move(additionalInfo)),
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

Profiler::Marker::Marker(const char* name, std::optional<std::string> additionalInfo) :
    m_Context(GetCPUContext()),
    m_Frame(GraphicsBackend::Current()->GetFrameNumber())
{
    MarkerInfo info(MarkerType::MARKER, name, std::move(additionalInfo), IncrementDepth(m_Context), m_Frame);
    m_MarkerIndex = AddMarkerInfo(m_Context, info, m_Frame);
}

Profiler::Marker::~Marker()
{
    SetMarkerEndTime(m_Context, m_Frame, m_MarkerIndex, std::chrono::system_clock::now());
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

    for (int i = 0; i < k_ContextCount; ++i)
    {
        std::lock_guard<std::mutex> lock(s_ContextMutexes[i]);
        std::map<uint64_t, Profiler::FrameInfo>& contextFrames = s_ContextFrames[i];

        if (contextFrames.size() >= k_MaxFrames)
            contextFrames.erase(contextFrames.begin());

        FrameInfo newFrame;
        newFrame.Frame = GraphicsBackend::Current()->GetFrameNumber();
        contextFrames[newFrame.Frame] = std::move(newFrame);
    }

    FrameInfo& newMainThreadFrame = s_ContextFrames[static_cast<int>(MarkerContext::MAIN_THREAD)][GraphicsBackend::Current()->GetFrameNumber()];
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

                MarkerInfo markerInfo(MarkerType::MARKER, gpuMarker.Name, std::nullopt, gpuMarker.Depth, gpuMarker.Frame);
                markerInfo.Begin = std::chrono::system_clock::time_point(std::chrono::microseconds(result.StartTimestamp));
                markerInfo.End = std::chrono::system_clock::time_point(std::chrono::microseconds(result.EndTimestamp));
                markerInfo.Finished = true;
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

int32_t Profiler::AddMarkerInfo(MarkerContext context, MarkerInfo& markerInfo, uint64_t frame)
{
    if (!s_IsEnabled)
        return -1;

    std::lock_guard<std::mutex> lock(GetContextMutex(context));
    std::map<uint64_t, FrameInfo>& contextFrames = s_ContextFrames[static_cast<int>(context)];

    auto it = contextFrames.find(frame);
    if (it != contextFrames.end())
    {
        FrameInfo& frameInfo = it->second;
        frameInfo.Markers.push_back(std::move(markerInfo));
        frameInfo.IsSorted = false;
        return frameInfo.Markers.size() - 1;
    }

    return -1;
}

void Profiler::SetMarkerEndTime(MarkerContext context, uint64_t frame, int32_t markerIndex, std::chrono::system_clock::time_point endTime)
{
    if (markerIndex == -1)
        return;

    std::lock_guard<std::mutex> lock(GetContextMutex(context));
    std::map<uint64_t, FrameInfo>& contextFrames = s_ContextFrames[static_cast<int>(context)];

    auto it = contextFrames.find(frame);
    if (it != contextFrames.end())
    {
        FrameInfo& frameInfo = it->second;
        if (frameInfo.Markers.size() > markerIndex)
        {
            frameInfo.Markers[markerIndex].End = endTime;
            frameInfo.Markers[markerIndex].Finished = true;
        }
    }
}

std::map<uint64_t, Profiler::FrameInfo>& Profiler::GetContextFrames(MarkerContext context)
{
    return s_ContextFrames[static_cast<int>(context)];
}

std::mutex& Profiler::GetContextMutex(Profiler::MarkerContext context)
{
    return s_ContextMutexes[static_cast<int>(context)];
}
