#ifndef RENDER_ENGINE_UI_EVENT_INFO_H
#define RENDER_ENGINE_UI_EVENT_INFO_H

#include "vector2/vector2.h"
#include "input/input.h"

enum class UIEventType
{
    POINTER_DOWN = 0,
    POINTER_UP = 1,
    HOVER = 2,
    CHAR_INPUT = 3,
    SPECIAL_KEY = 4,
};

struct UIEventInfo
{
    union{
        Vector2 Position{};
        Input::KeyboardKeyState KeyState;
    };
    bool Consumed = false;
    bool LoseFocus = false;
    UIEventType Type;
};

#endif //RENDER_ENGINE_UI_EVENT_INFO_H
