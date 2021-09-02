#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "input.h"
#include "GLUT/glut.h"

void Input::Init()
{
    m_Instance = make_unique<Input>();
    glutPassiveMotionFunc(m_Instance->MouseMove);
    glutKeyboardFunc(m_Instance->KeyboardDown);
    glutKeyboardUpFunc(m_Instance->KeyboardUp);
}

void Input::MouseMove(int _x, int _y)
{
    if (m_Instance != nullptr)
        m_Instance->m_MousePosition = Vector3((float) _x, (float) _y, 0);
}

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "UnusedParameter"

void Input::KeyboardDown(unsigned char _key, int _x, int _y)
{
    if (m_Instance != nullptr && !m_Instance->m_Inputs.contains(_key))
        m_Instance->m_Inputs.insert(_key);
}

void Input::KeyboardUp(unsigned char _key, int _x, int _y)
{
    if (m_Instance != nullptr && m_Instance->m_Inputs.contains(_key))
        m_Instance->m_Inputs.erase(_key);
}

#pragma clang diagnostic pop

void Input::Update()
{
    if (m_Instance != nullptr)
        m_Instance->Update_Internal();
}

void Input::Update_Internal()
{
    m_MouseDelta       = m_OldMousePosition - m_MousePosition;
    m_OldMousePosition = m_MousePosition;
}

bool Input::IsKeyDown(unsigned char _key)
{
    return m_Instance != nullptr && m_Instance->m_Inputs.contains(_key);
}

Vector3 Input::GetMousePosition()
{
    return m_Instance != nullptr ? m_Instance->m_MousePosition : Vector3();
}

Vector3 Input::GetMouseDelta()
{
    return m_Instance != nullptr ? m_Instance->m_MouseDelta : Vector3();
}