#ifndef PROFILER_MARKERS_H
#define PROFILER_MARKERS_H

#include <chrono>

class Profiler
{
public:
    struct MarkerInfo
    {
        std::chrono::high_resolution_clock::time_point Begin;
        std::chrono::high_resolution_clock::time_point End;
        const char* Name = nullptr;
        const int Depth = 0;
    };

    struct Marker
    {
        MarkerInfo Info;

        explicit Marker(const char* name);
        ~Marker();
    };

    static void SetEnabled(bool enabled);
    static void BeginNewFrame();
    static void AddMarkerInfo(const MarkerInfo& markerInfo);
    static std::vector<std::vector<MarkerInfo>>& GetMarkerInfos();
};

#endif //PROFILER_MARKERS_H
