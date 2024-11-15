#ifndef RENDER_ENGINE_INPUT_H
#define RENDER_ENGINE_INPUT_H

#include <cstdint>
#include <vector>

#include "vector2/vector2.h"

namespace Input
{
    enum class TouchState
    {
        DOWN,
        MOVE,
        UP
    };

    struct Touch
    {
        uint64_t Id{};
        TouchState State = TouchState::DOWN;
        Vector2 Position{};
        Vector2 Delta{};
    };

    void Update();
    void CleanUp();

    void HandleKeyboardInput(unsigned char key, bool isPressed);
    void HandleMouseMove(double x, double y);
    void HandleTouch(TouchState state, uint64_t touchId, float x, float y);

    bool GetKeyDown(unsigned char key);
    bool GetKeyUp(unsigned char key);
    bool GetKey(unsigned char key);
    bool GetTouch(uint64_t touchId, Touch& outTouch);
    const Vector2& GetMousePosition();
    const Vector2& GetMouseDelta();
    const std::vector<Touch>& GetTouches();
}; // namespace Input

#endif //RENDER_ENGINE_INPUT_H
