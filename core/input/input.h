#ifndef OPENGL_STUDY_INPUT_H
#define OPENGL_STUDY_INPUT_H

#include "../../math/vector3/vector3.h"
#include <unordered_set>

using namespace std;

class Input
{
public:
    static void    Init();
    static void    Update();
    static void    CleanUp();
    static bool    GetKeyDown(unsigned char _key);
    static bool    GetKeyUp(unsigned char _key);
    static bool    GetKey(unsigned char _key);
    static Vector3 GetMousePosition();
    static Vector3 GetMouseDelta();

private:
    inline static unique_ptr<Input> m_Instance = nullptr;
    unordered_set<unsigned char>    m_Inputs;
    unordered_set<unsigned char>    m_InputsDown;
    unordered_set<unsigned char>    m_InputsUp;
    Vector3                         m_OldMousePosition = Vector3();
    Vector3                         m_MousePosition    = Vector3();
    Vector3                         m_MouseDelta       = Vector3();

    void Update_Internal();
    void CleanUp_Internal();

    static void MouseMove(int _x, int _y);
    static void KeyboardDown(unsigned char _key, int _x, int _y);
    static void KeyboardUp(unsigned char _key, int _x, int _y);
};

#endif //OPENGL_STUDY_INPUT_H
