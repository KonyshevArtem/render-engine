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
    static bool    IsKeyDown(unsigned char key);
    static Vector3 GetMousePosition();
    static Vector3 GetMouseDelta();

private:
    inline static unique_ptr<Input> Instance = nullptr;
    unordered_set<unsigned char>    Inputs;
    Vector3                         OldMousePosition = Vector3::Zero();
    Vector3                         MousePosition    = Vector3::Zero();
    Vector3                         MouseDelta       = Vector3::Zero();

    void Update_Internal();

    static void MouseMove(int x, int y);
    static void KeyboardDown(unsigned char key, int x, int y);
    static void KeyboardUp(unsigned char key, int x, int y);
};

#endif //OPENGL_STUDY_INPUT_H
