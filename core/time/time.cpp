#define GL_SILENCE_DEPRECATION

#include "time.h" // NOLINT(modernize-deprecated-headers)
#include "GLUT/glut.h"

void Time::Update()
{
    TimePassed = (float) glutGet(GLUT_ELAPSED_TIME);
    DeltaTime  = TimePassed - PrevTime;
    PrevTime   = TimePassed;
}
