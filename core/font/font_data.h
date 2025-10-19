#ifndef RENDER_ENGINE_FONT_DATA_H
#define RENDER_ENGINE_FONT_DATA_H

#include <cstdint>

struct CommonBlock
{
    uint16_t LineHeight;
    uint16_t Base;
    uint16_t ScaleW;
    uint16_t ScaleH;
    uint16_t Pages;
    uint8_t BitField;
    uint8_t AlphaChannel;
    uint8_t RedChannel;
    uint8_t GreenChannel;
    uint8_t BlueChannel;
    uint16_t CharsCount;
    uint16_t KerningPairsCount;
};

struct Char
{
    uint32_t Id;
    uint16_t X;
    uint16_t Y;
    uint16_t Width;
    uint16_t Height;
    int16_t XOffset;
    int16_t YOffset;
    int16_t XAdvance;
    uint8_t Page;
    uint8_t Channel;
};

struct KerningPair
{
    uint32_t First;
    uint32_t Second;
    int16_t Amount;
};

#endif //RENDER_ENGINE_FONT_DATA_H
