#ifndef RENDER_ENGINE_UI_EVENT_INFO_H
#define RENDER_ENGINE_UI_EVENT_INFO_H

#include "vector2/vector2.h"

enum UIEventType
{
    POINTER_DOWN = 0,
    POINTER_UP = 1,
    HOVER = 2,
};

struct UIEventInfo
{
    Vector2 Position{};
    bool Consumed = false;
    UIEventType Type;
};

#endif //RENDER_ENGINE_UI_EVENT_INFO_H
