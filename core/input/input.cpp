#include "input.h"
#include <unordered_set>

namespace Input
{
    std::vector<Touch> s_Touches;
    std::unordered_set<unsigned char> s_Inputs;
    std::unordered_set<unsigned char> s_InputsDown;
    std::unordered_set<unsigned char> s_InputsUp;
    Vector2 s_OldMousePosition = Vector2();
    Vector2 s_MousePosition = Vector2();
    Vector2 s_MouseDelta = Vector2();

    void Update()
    {
        s_MouseDelta = s_OldMousePosition - s_MousePosition;
        s_OldMousePosition = s_MousePosition;
    }

    void CleanUp()
    {
        s_InputsUp.clear();
        s_InputsDown.clear();

        for (int i = 0; i < s_Touches.size(); ++i)
        {
            if (s_Touches[i].State == TouchState::UP)
            {
                s_Touches[i] = s_Touches[s_Touches.size() - 1];
                s_Touches.pop_back();
                --i;
            }
        }
    }

    void HandleKeyboardInput(unsigned char key, bool isPressed)
    {
        if (isPressed)
        {
            if (!s_Inputs.contains(key))
            {
                s_Inputs.insert(key);
                s_InputsDown.insert(key);
            }
        }
        else
        {
            if (s_Inputs.contains(key))
            {
                s_Inputs.erase(key);
                s_InputsUp.insert(key);
            }
        }
    }

    void HandleMouseMove(double x, double y)
    {
        s_MousePosition = Vector2(static_cast<float>(x), static_cast<float>(y));
    }

    void HandleTouch(TouchState state, uint64_t touchId, float x, float y)
    {
        if (state == TouchState::DOWN)
        {
            const Touch touch{touchId, TouchState::DOWN, {x, y}, {0, 0}};
            s_Touches.push_back(touch);
        }

        if (state == TouchState::MOVE)
        {
            for (Touch& touch : s_Touches)
            {
                if (touch.Id == touchId)
                {
                    Vector2 pos = {x, y};
                    touch.Delta = pos - touch.Position;
                    touch.Position = pos;
                    touch.State = TouchState::MOVE;
                    break;
                }
            }
        }

        if (state == TouchState::UP)
        {
            for (Touch& touch : s_Touches)
            {
                if (touch.Id == touchId)
                {
                    touch.State = TouchState::UP;
                    break;
                }
            }
        }
    }

    bool GetKeyDown(unsigned char key)
    {
        return s_InputsDown.contains(key);
    }
    bool GetKeyUp(unsigned char key)
    {
        return s_InputsUp.contains(key);
    }

    bool GetKey(unsigned char key)
    {
        return s_Inputs.contains(key);
    }

    bool GetTouch(uint64_t touchId, Touch& outTouch)
    {
        for (const Touch& touch : s_Touches)
        {
            if (touch.Id == touchId)
            {
                outTouch = touch;
                return true;
            }
        }

        return false;
    }

    const Vector2 &GetMousePosition()
    {
        return s_MousePosition;
    }

    const Vector2 &GetMouseDelta()
    {
        return s_MouseDelta;
    }

    const std::vector<Touch> & GetTouches()
    {
        return s_Touches;
    }
} // namespace Input