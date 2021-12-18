#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "time.h" // NOLINT(modernize-deprecated-headers)
#include "GLUT/glut.h"

float Time::m_ElapsedTime = 0;
float Time::m_DeltaTime   = 0;
float Time::m_PrevTime    = 0;

void Time::Update()
{
    m_ElapsedTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME));
    m_DeltaTime   = m_ElapsedTime - m_PrevTime;
    m_PrevTime    = m_ElapsedTime;
}
