#include <chrono>
#include <time.h> // NOLINT(modernize-deprecated-headers)

namespace Time
{
    std::chrono::steady_clock::time_point startupTime;
    double                                elapsedTime = 0;
    double                                deltaTime   = 0;
    double                                prevTime    = 0;

    void Init()
    {
        startupTime = std::chrono::steady_clock::now();
    }

    void Update()
    {
        auto now    = std::chrono::steady_clock::now();
        elapsedTime = std::chrono::duration<double>(now - startupTime).count();
        deltaTime   = elapsedTime - prevTime;
        prevTime    = elapsedTime;
    }

    double GetElapsedTime()
    {
        return elapsedTime;
    }

    double GetDeltaTime()
    {
        return deltaTime;
    }
} // namespace Time
