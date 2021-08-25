#define GL_SILENCE_DEPRECATION

#include "input.h"
#include "GLUT/glut.h"

void Input::Init()
{
    Instance = make_unique<Input>();
    glutPassiveMotionFunc(Instance->MouseMove);
    glutKeyboardFunc(Instance->KeyboardDown);
    glutKeyboardUpFunc(Instance->KeyboardUp);
}

void Input::MouseMove(int x, int y)
{
    if (Instance != nullptr)
        Instance->MousePosition = Vector3((float) x, (float) y, 0);
}

void Input::KeyboardDown(unsigned char key, int x, int y)
{
    if (Instance != nullptr && !Instance->Inputs.contains(key))
        Instance->Inputs.insert(key);
}

void Input::KeyboardUp(unsigned char key, int x, int y)
{
    if (Instance != nullptr && Instance->Inputs.contains(key))
        Instance->Inputs.erase(key);
}

void Input::Update()
{
    if (Instance != nullptr)
        Instance->Update_Internal();
}

void Input::Update_Internal()
{
    MouseDelta       = OldMousePosition - MousePosition;
    OldMousePosition = MousePosition;
}

bool Input::IsKeyDown(unsigned char key)
{
    return Instance != nullptr && Instance->Inputs.contains(key);
}

Vector3 Input::GetMousePosition()
{
    return Instance != nullptr ? Instance->MousePosition : Vector3::Zero();
}

Vector3 Input::GetMouseDelta()
{
    return Instance != nullptr ? Instance->MouseDelta : Vector3::Zero();
}
