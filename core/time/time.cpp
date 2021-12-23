#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "time.h" // NOLINT(modernize-deprecated-headers)
#include "GLUT/glut.h"

namespace Time
{
    float elapsedTime = 0;
    float deltaTime   = 0;
    float prevTime    = 0;

    void Update()
    {
        elapsedTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME));
        deltaTime   = elapsedTime - prevTime;
        prevTime    = elapsedTime;
    }

    float GetElapsedTime()
    {
        return elapsedTime;
    }

    float GetDeltaTime()
    {
        return deltaTime;
    }
} // namespace Time
