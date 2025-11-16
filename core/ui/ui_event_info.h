#ifndef RENDER_ENGINE_UI_EVENT_INFO_H
#define RENDER_ENGINE_UI_EVENT_INFO_H

#include "vector2/vector2.h"

struct UIEventInfo
{
    Vector2 Position{};
    bool Consumed = false;
};

#endif //RENDER_ENGINE_UI_EVENT_INFO_H
