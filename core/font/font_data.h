#ifndef RENDER_ENGINE_FONT_DATA_H
#define RENDER_ENGINE_FONT_DATA_H

#include <cstdint>

struct CommonBlock
{
    uint16_t LineHeight;
    uint16_t ScaleW;
    uint16_t ScaleH;
};

struct Char
{
    uint16_t X;
    uint16_t Y;
    uint16_t Width;
    uint16_t Height;
    int16_t XOffset;
    int16_t YOffset;
    int16_t XAdvance;
};

#endif //RENDER_ENGINE_FONT_DATA_H
