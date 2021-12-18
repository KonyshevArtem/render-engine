#ifndef OPENGL_STUDY_INPUT_H
#define OPENGL_STUDY_INPUT_H

#include "../../math/vector2/vector2.h"
#include <unordered_set>

using namespace std;

class Input
{
public:
    static void           Init();
    static void           Update();
    static void           CleanUp();
    static bool           GetKeyDown(unsigned char _key);
    static bool           GetKeyUp(unsigned char _key);
    static bool           GetKey(unsigned char _key);
    static const Vector2 &GetMousePosition();
    static const Vector2 &GetMouseDelta();

private:
    Input()              = default;
    Input(const Input &) = delete;

    inline static unique_ptr<Input> m_Instance = nullptr;
    unordered_set<unsigned char>    m_Inputs;
    unordered_set<unsigned char>    m_InputsDown;
    unordered_set<unsigned char>    m_InputsUp;
    Vector2                         m_OldMousePosition = Vector2();
    Vector2                         m_MousePosition    = Vector2();
    Vector2                         m_MouseDelta       = Vector2();

    void Update_Internal();
    void CleanUp_Internal();

    static void MouseMove(int _x, int _y);
    static void KeyboardDown(unsigned char _key, int _x, int _y);
    static void KeyboardUp(unsigned char _key, int _x, int _y);
};

#endif //OPENGL_STUDY_INPUT_H
