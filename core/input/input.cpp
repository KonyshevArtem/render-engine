#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "input.h"
#include <GLUT/glut.h>

void Input::Init()
{
    m_Instance = unique_ptr<Input>(new Input());
    glutPassiveMotionFunc(m_Instance->MouseMove);
    glutKeyboardFunc(m_Instance->KeyboardDown);
    glutKeyboardUpFunc(m_Instance->KeyboardUp);
}

void Input::MouseMove(int _x, int _y)
{
    if (m_Instance != nullptr)
        m_Instance->m_MousePosition = Vector2(static_cast<float>(_x), static_cast<float>(_y));
}

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "UnusedParameter"

void Input::KeyboardDown(unsigned char _key, int _x, int _y)
{
    if (m_Instance != nullptr && !m_Instance->m_Inputs.contains(_key))
    {
        m_Instance->m_Inputs.insert(_key);
        m_Instance->m_InputsDown.insert(_key);
    }
}

void Input::KeyboardUp(unsigned char _key, int _x, int _y)
{
    if (m_Instance != nullptr && m_Instance->m_Inputs.contains(_key))
    {
        m_Instance->m_Inputs.erase(_key);
        m_Instance->m_InputsUp.insert(_key);
    }
}

#pragma clang diagnostic pop

void Input::Update()
{
    if (m_Instance != nullptr)
        m_Instance->Update_Internal();
}

void Input::CleanUp()
{
    if (m_Instance != nullptr)
        m_Instance->CleanUp_Internal();
}

void Input::Update_Internal()
{
    m_MouseDelta       = m_OldMousePosition - m_MousePosition;
    m_OldMousePosition = m_MousePosition;
}

void Input::CleanUp_Internal()
{
    m_InputsUp.clear();
    m_InputsDown.clear();
}

bool Input::GetKeyDown(unsigned char _key)
{
    return m_Instance != nullptr && m_Instance->m_InputsDown.contains(_key);
}
bool Input::GetKeyUp(unsigned char _key)
{
    return m_Instance != nullptr && m_Instance->m_InputsUp.contains(_key);
}

bool Input::GetKey(unsigned char _key)
{
    return m_Instance != nullptr && m_Instance->m_Inputs.contains(_key);
}

const Vector2 &Input::GetMousePosition()
{
    return m_Instance != nullptr ? m_Instance->m_MousePosition : Vector2::Zero();
}

const Vector2 &Input::GetMouseDelta()
{
    return m_Instance != nullptr ? m_Instance->m_MouseDelta : Vector2::Zero();
}