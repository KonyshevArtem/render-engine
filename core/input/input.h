#ifndef RENDER_ENGINE_INPUT_H
#define RENDER_ENGINE_INPUT_H

#include <cstdint>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "vector2/vector2.h"

namespace Input
{
    enum class TouchState
    {
        DOWN,
        MOVE,
        UP
    };

    enum KeyState
    {
        DOWN = 1 << 0,
        PRESSED = 1 << 1,
        UP = 1 << 2
    };

    enum class MouseButton
    {
        LEFT,
        RIGHT,
        MIDDLE
    };

    enum class SpecialKey
    {
        LEFT_SHIFT,
        RIGHT_SHIFT,

        LEFT_CTRL,
        RIGHT_CTRL,

        ENTER,

        BACKSPACE,
        DELETE,

        HOME,
        END,

        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    struct KeyboardKeyState
    {
        union
        {
            unsigned char Char;
            SpecialKey SpecialKey;
        };
        uint16_t State;

        bool IsDown() const;
        bool IsPressed() const;
        bool IsUp() const;
    };

    struct Touch
    {
        uint64_t Id{};
        uint64_t StartFrame;
        TouchState State = TouchState::DOWN;
        Vector2 Position{};
        Vector2 Delta{};
    };

    void Update();
    void CleanUp();

    void HandleKeyboardInput(unsigned char key, bool isPressed);
    void HandleCharInput(unsigned char ch);
    void HandleSpecialKeyInput(int keyId, bool isPressed);
    void HandleMouseClick(MouseButton mouseButton, bool isPressed);
    void HandleMouseMove(double x, double y);
    void HandleTouch(TouchState state, uint64_t touchId, float x, float y);

    const std::unordered_set<unsigned char>& GetCharInputs();
    const std::unordered_map<SpecialKey, KeyboardKeyState>& GetSpecialKeys();
    bool GetKeyDown(unsigned char key);
    bool GetKeyUp(unsigned char key);
    bool GetKey(unsigned char key);

    bool GetTouch(uint64_t touchId, Touch& outTouch);
    const std::vector<Touch>& GetTouches();

    bool GetMouseButton(MouseButton mouseButton);
    bool GetMouseButtonDown(MouseButton mouseButton);
    bool GetMouseButtonUp(MouseButton mouseButton);

    const Vector2& GetMousePosition();
    const Vector2& GetMouseDelta();
}; // namespace Input

#endif //RENDER_ENGINE_INPUT_H
