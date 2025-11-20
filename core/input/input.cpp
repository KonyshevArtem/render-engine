#include "input.h"
#include <unordered_set>

namespace Input
{
    std::vector<Touch> s_Touches;
    std::vector<Touch> s_PendingTouches;
    std::unordered_set<unsigned char> s_Inputs;
    std::unordered_set<unsigned char> s_InputsDown;
    std::unordered_set<unsigned char> s_InputsUp;
    std::unordered_set<unsigned char> s_CharInputs;
    Vector2 s_OldMousePosition = Vector2();
    Vector2 s_MousePosition = Vector2();
    Vector2 s_MouseDelta = Vector2();
    uint8_t s_MouseButtonBits;
    uint8_t s_MouseButtonDownBits;
    uint8_t s_MouseButtonUpBits;

    void ProcessPendingTouches()
    {
        for (const Touch& pendingTouch: s_PendingTouches)
        {
            if (pendingTouch.State == TouchState::DOWN)
            {
                const Touch touch{pendingTouch.Id, TouchState::DOWN, {pendingTouch.Position.x, pendingTouch.Position.y}, {0, 0}};
                s_Touches.push_back(touch);
            }

            if (pendingTouch.State == TouchState::MOVE)
            {
                for (Touch& touch: s_Touches)
                {
                    if (touch.Id == pendingTouch.Id)
                    {
                        Vector2 pos = {pendingTouch.Position.x, pendingTouch.Position.y};
                        touch.Delta = pos - touch.Position;
                        touch.Position = pos;
                        touch.State = TouchState::MOVE;
                        break;
                    }
                }
            }

            if (pendingTouch.State == TouchState::UP)
            {
                for (Touch& touch: s_Touches)
                {
                    if (touch.Id == pendingTouch.Id)
                    {
                        touch.State = TouchState::UP;
                        break;
                    }
                }
            }
        }

        s_PendingTouches.clear();
    }

    void Update()
    {
        s_MouseDelta = s_OldMousePosition - s_MousePosition;
        s_OldMousePosition = s_MousePosition;

        ProcessPendingTouches();
    }

    void CleanUp()
    {
        s_InputsUp.clear();
        s_InputsDown.clear();
        s_CharInputs.clear();

        s_MouseButtonDownBits = 0;
        s_MouseButtonUpBits = 0;

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

    void HandleCharInput(unsigned char ch)
    {
        if (!s_CharInputs.contains(ch))
            s_CharInputs.insert(ch);
    }

    void HandleMouseClick(MouseButton mouseButton, bool isPressed)
    {
        uint8_t mouseButtonBit = 1U << static_cast<int>(mouseButton);
        if (isPressed)
        {
            s_MouseButtonBits |= mouseButtonBit;
            s_MouseButtonDownBits |= mouseButtonBit;
        }
        else
        {
            s_MouseButtonBits &= ~mouseButtonBit;
            s_MouseButtonUpBits |= mouseButtonBit;
        }
    }

    void HandleMouseMove(double x, double y)
    {
        s_MousePosition = Vector2(static_cast<float>(x), static_cast<float>(y));
    }

    void HandleTouch(TouchState state, uint64_t touchId, float x, float y)
    {
        s_PendingTouches.push_back(Touch{touchId, state, {x, y}, {0, 0}});
    }

    const std::unordered_set<unsigned char>& GetCharInputs()
    {
        return s_CharInputs;
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

    bool GetMouseButton(MouseButton mouseButton)
    {
        uint8_t mouseButtonBit = 1U << static_cast<int>(mouseButton);
        return (s_MouseButtonBits & mouseButtonBit) != 0;
    }

    bool GetMouseButtonDown(MouseButton mouseButton)
    {
        uint8_t mouseButtonBit = 1U << static_cast<int>(mouseButton);
        return (s_MouseButtonDownBits & mouseButtonBit) != 0;
    }

    bool GetMouseButtonUp(MouseButton mouseButton)
    {
        uint8_t mouseButtonBit = 1U << static_cast<int>(mouseButton);
        return (s_MouseButtonUpBits & mouseButtonBit) != 0;
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