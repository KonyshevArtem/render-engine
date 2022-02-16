#include "input.h"
#include "vector2/vector2.h"
#include <unordered_set>
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/freeglut.h>
#elif OPENGL_STUDY_MACOS
#include <GLUT/glut.h>
#endif

namespace Input
{
    std::unordered_set<unsigned char> inputs;
    std::unordered_set<unsigned char> inputsDown;
    std::unordered_set<unsigned char> inputsUp;
    Vector2                           oldMousePosition = Vector2();
    Vector2                           mousePosition    = Vector2();
    Vector2                           mouseDelta       = Vector2();

    void MouseMove(int _x, int _y)
    {
        mousePosition = Vector2(static_cast<float>(_x), static_cast<float>(_y));
    }

    void KeyboardDown(unsigned char _key, int, int)
    {
        if (!inputs.contains(_key))
        {
            inputs.insert(_key);
            inputsDown.insert(_key);
        }
    }

    void KeyboardUp(unsigned char _key, int, int)
    {
        if (inputs.contains(_key))
        {
            inputs.erase(_key);
            inputsUp.insert(_key);
        }
    }

    void Init()
    {
        glutPassiveMotionFunc(MouseMove);
        glutKeyboardFunc(KeyboardDown);
        glutKeyboardUpFunc(KeyboardUp);
    }

    void Update()
    {
        mouseDelta       = oldMousePosition - mousePosition;
        oldMousePosition = mousePosition;
    }

    void CleanUp()
    {
        inputsUp.clear();
        inputsDown.clear();
    }

    bool GetKeyDown(unsigned char _key)
    {
        return inputsDown.contains(_key);
    }
    bool GetKeyUp(unsigned char _key)
    {
        return inputsUp.contains(_key);
    }

    bool GetKey(unsigned char _key)
    {
        return inputs.contains(_key);
    }

    const Vector2 &GetMousePosition()
    {
        return mousePosition;
    }

    const Vector2 &GetMouseDelta()
    {
        return mouseDelta;
    }
} // namespace Input