#include "input.h"
#include "vector2/vector2.h"
#include <unordered_set>

namespace Input
{
    std::unordered_set<unsigned char> inputs;
    std::unordered_set<unsigned char> inputsDown;
    std::unordered_set<unsigned char> inputsUp;
    Vector2                           oldMousePosition = Vector2();
    Vector2                           mousePosition    = Vector2();
    Vector2                           mouseDelta       = Vector2();

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

    void HandleKeyboardInput(unsigned char _key, bool _isPressed)
    {
        if (_isPressed)
        {
            if (!inputs.contains(_key))
            {
                inputs.insert(_key);
                inputsDown.insert(_key);
            }
        }
        else
        {
            if (inputs.contains(_key))
            {
                inputs.erase(_key);
                inputsUp.insert(_key);
            }
        }
    }

    void HandleMouseMove(double _x, double _y)
    {
        mousePosition = Vector2(static_cast<float>(_x), static_cast<float>(_y));
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