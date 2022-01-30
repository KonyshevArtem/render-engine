#include <time.h> // NOLINT(modernize-deprecated-headers)
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/freeglut.h>
#elif OPENGL_STUDY_MACOS
#include <GLUT/glut.h>
#endif

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
