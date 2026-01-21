#include "input.h"
#include "graphics_backend_api.h"

namespace Input
{
    std::vector<Touch> s_Touches;
    std::vector<Touch> s_PendingTouches;
    std::unordered_map<unsigned char, KeyboardKeyState> s_KeyboardKeys;
    std::unordered_map<SpecialKey, KeyboardKeyState> s_SpecialKeys;
    std::unordered_set<wchar_t> s_CharInputs;
    Vector2 s_OldMousePosition = Vector2();
    Vector2 s_MousePosition = Vector2();
    Vector2 s_MouseDelta = Vector2();
    uint8_t s_MouseButtonBits;
    uint8_t s_MouseButtonDownBits;
    uint8_t s_MouseButtonUpBits;

    bool KeyboardKeyState::IsDown() const
    {
        return (State & KeyState::DOWN) != 0;
    }

    bool KeyboardKeyState::IsPressed() const
    {
        return (State & KeyState::PRESSED) != 0;
    }

    bool KeyboardKeyState::IsUp() const
    {
        return (State & KeyState::UP) != 0;
    }

    void ProcessPendingTouches()
    {
        for (const Touch& pendingTouch: s_PendingTouches)
        {
            if (pendingTouch.State == TouchState::DOWN)
            {
                const Touch touch{pendingTouch.Id, pendingTouch.StartFrame, TouchState::DOWN, {pendingTouch.Position.x, pendingTouch.Position.y}, {0, 0}};
                s_Touches.push_back(touch);
            }

            if (pendingTouch.State == TouchState::MOVE)
            {
                for (Touch& touch: s_Touches)
                {
                    if (touch.Id == pendingTouch.Id && pendingTouch.StartFrame > touch.StartFrame)
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
                        touch.State = TouchState::UP;
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
        auto UpdateKeyState = [](KeyboardKeyState& keyState)
        {
            if (keyState.IsUp())
                keyState.State = 0;
            keyState.State &= ~KeyState::DOWN;
        };

        for (auto& pair : s_KeyboardKeys)
            UpdateKeyState(pair.second);

        for (auto& pair : s_SpecialKeys)
            UpdateKeyState(pair.second);

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
        KeyboardKeyState keyState{};
        keyState.Char = key;
        keyState.State = KeyState::PRESSED | (isPressed ? KeyState::DOWN : KeyState::UP);
        s_KeyboardKeys[key] = keyState;
    }

    void HandleCharInput(wchar_t ch)
    {
        if (!s_CharInputs.contains(ch))
            s_CharInputs.insert(ch);
    }

    void HandleSpecialKeyInput(int keyId, bool isPressed)
    {
        KeyboardKeyState keyState{};
        keyState.SpecialKey = static_cast<SpecialKey>(keyId);
        keyState.State = KeyState::PRESSED | (isPressed ? KeyState::DOWN : KeyState::UP);
        s_SpecialKeys[keyState.SpecialKey] = keyState;
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
        s_PendingTouches.push_back(Touch{touchId, GraphicsBackend::Current()->GetFrameNumber(), state, {x, y}, {0, 0}});
    }

    const std::unordered_set<wchar_t>& GetCharInputs()
    {
        return s_CharInputs;
    }

    const std::unordered_map<SpecialKey, KeyboardKeyState>& GetSpecialKeys()
    {
        return s_SpecialKeys;
    }

    bool CheckKeyState(unsigned char key, KeyState state)
    {
        const auto& it = s_KeyboardKeys.find(key);
        if (it != s_KeyboardKeys.end())
            return (it->second.State & state) != 0;
        return false;
    }

    bool GetKeyDown(unsigned char key)
    {
        return CheckKeyState(key, KeyState::DOWN);
    }

    bool GetKeyUp(unsigned char key)
    {
        return CheckKeyState(key, KeyState::UP);
    }

    bool GetKey(unsigned char key)
    {
        return CheckKeyState(key, KeyState::PRESSED);
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