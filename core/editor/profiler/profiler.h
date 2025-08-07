#ifndef PROFILER_MARKERS_H
#define PROFILER_MARKERS_H

#include "types/graphics_backend_profiler_marker.h"

#include <chrono>
#include <vector>
#include <mutex>
#include <map>

class Profiler
{
public:
    enum class MarkerType
    {
        MARKER,
        SEPARATOR
    };

    enum class MarkerContext
    {
        MAIN_THREAD,
        GPU_RENDER,
        GPU_COPY,
        WORKER_1,
        WORKER_2,
        WORKER_3,
        WORKER_4,
        WORKER_5,

        MAX
    };

    struct MarkerInfo
    {
        MarkerType Type = MarkerType::MARKER;
        std::chrono::system_clock::time_point Begin;
        std::chrono::system_clock::time_point End;
        const char* Name = nullptr;
        std::optional<std::string> AdditionalInfo = std::nullopt;
        int Depth = 0;
        uint64_t Frame = 0;
        bool Finished = false;

        explicit MarkerInfo(MarkerType type, const char* name = nullptr, std::optional<std::string> additionalInfo = std::nullopt, int depth = 0, uint64_t frame = 0);

        MarkerInfo(const MarkerInfo& other) = default;
        MarkerInfo(MarkerInfo&& info) = default;

        MarkerInfo& operator=(const MarkerInfo& other)
        {
            if (&other != this)
            {
                Type = other.Type;
                Begin = other.Begin;
                End = other.End;
                Name = other.Name;
                AdditionalInfo = other.AdditionalInfo;
                Depth = other.Depth;
            }
            return *this;
        }

        MarkerInfo& operator=(MarkerInfo&& other)
        {
            if (&other != this)
            {
                Type = other.Type;
                Begin = other.Begin;
                End = other.End;
                Name = other.Name;
                AdditionalInfo = std::move(other.AdditionalInfo);
                Depth = other.Depth;
            }
            return *this;
        }
    };

    struct GPUMarkerInfo
    {
        GraphicsBackendProfilerMarker ProfilerMarker;
        const char* Name = nullptr;
        int Depth = 0;
        uint64_t Frame = 0;

        GPUMarkerInfo(const char* name, int depth, uint64_t frame);
        GPUMarkerInfo& operator=(const GPUMarkerInfo& info);
    };

    struct Marker
    {
        explicit Marker(const char* name, std::optional<std::string> additionalData = std::nullopt);
        ~Marker();

    private:
        MarkerContext m_Context;
        uint32_t m_MarkerIndex;
        uint64_t m_Frame;
    };

    struct GPUMarker
    {
        explicit GPUMarker(const char* name);
        ~GPUMarker();

    private:
        MarkerContext m_Context;
        GPUMarkerInfo m_Info;
    };

    struct FrameInfo
    {
        std::vector<MarkerInfo> Markers;
        uint64_t Frame;
        bool IsSorted;
    };

    static void SetEnabled(bool enabled);
    static void BeginNewFrame();
    static std::map<uint64_t, FrameInfo>& GetContextFrames(MarkerContext context);
    static std::mutex& GetContextMutex(MarkerContext context);

private:
    static int32_t AddMarkerInfo(MarkerContext context, MarkerInfo& markerInfo, uint64_t frame);
    static void SetMarkerEndTime(MarkerContext context, uint64_t frame, int32_t frameIndex, std::chrono::system_clock::time_point endTime);
};

#endif //PROFILER_MARKERS_H
