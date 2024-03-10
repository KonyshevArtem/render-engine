#ifndef RENDER_ENGINE_INPUT_H
#define RENDER_ENGINE_INPUT_H

struct Vector2;

namespace Input
{
    void Update();
    void CleanUp();
    void HandleKeyboardInput(unsigned char _key, bool _isPressed);
    void HandleMouseMove(double _x, double _y);

    bool           GetKeyDown(unsigned char _key);
    bool           GetKeyUp(unsigned char _key);
    bool           GetKey(unsigned char _key);
    const Vector2 &GetMousePosition();
    const Vector2 &GetMouseDelta();
}; // namespace Input

#endif //RENDER_ENGINE_INPUT_H
