#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "time.h" // NOLINT(modernize-deprecated-headers)
#include "GLUT/glut.h"

void Time::Update()
{
    TimePassed = static_cast<float>(glutGet(GLUT_ELAPSED_TIME));
    DeltaTime  = TimePassed - PrevTime;
    PrevTime   = TimePassed;
}
