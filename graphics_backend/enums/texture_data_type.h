#ifndef RENDER_ENGINE_TEXTURE_DATA_TYPE_H
#define RENDER_ENGINE_TEXTURE_DATA_TYPE_H

#include "graphics_backend.h"

enum class TextureDataType : GRAPHICS_BACKEND_TYPE_ENUM
{
    UNSIGNED_BYTE   = GRAPHICS_BACKEND_UNSIGNED_BYTE,
    BYTE            = GRAPHICS_BACKEND_BYTE,
    UNSIGNED_SHORT  = GRAPHICS_BACKEND_UNSIGNED_SHORT,
    SHORT           = GRAPHICS_BACKEND_SHORT,
    UNSIGNED_INT    = GRAPHICS_BACKEND_UNSIGNED_INT,
    INT             = GRAPHICS_BACKEND_INT,
    HALF_FLOAT      = GRAPHICS_BACKEND_HALF_FLOAT,
    FLOAT           = GRAPHICS_BACKEND_FLOAT,

    UNSIGNED_BYTE_3_3_2         = GRAPHICS_BACKEND_UNSIGNED_BYTE_3_3_2,
    UNSIGNED_BYTE_2_3_3_REV     = GRAPHICS_BACKEND_UNSIGNED_BYTE_2_3_3_REV,
    UNSIGNED_SHORT_5_6_5        = GRAPHICS_BACKEND_UNSIGNED_SHORT_5_6_5,
    UNSIGNED_SHORT_5_6_5_REV    = GRAPHICS_BACKEND_UNSIGNED_SHORT_5_6_5_REV,
    UNSIGNED_SHORT_4_4_4_4      = GRAPHICS_BACKEND_UNSIGNED_SHORT_4_4_4_4,
    UNSIGNED_SHORT_4_4_4_4_REV  = GRAPHICS_BACKEND_UNSIGNED_SHORT_4_4_4_4_REV,
    UNSIGNED_SHORT_5_5_5_1      = GRAPHICS_BACKEND_UNSIGNED_SHORT_5_5_5_1,
    UNSIGNED_SHORT_1_5_5_5_REV  = GRAPHICS_BACKEND_UNSIGNED_SHORT_1_5_5_5_REV,
    UNSIGNED_INT_8_8_8_8        = GRAPHICS_BACKEND_UNSIGNED_INT_8_8_8_8,
    UNSIGNED_INT_8_8_8_8_REV    = GRAPHICS_BACKEND_UNSIGNED_INT_8_8_8_8_REV,
    UNSIGNED_INT_10_10_10_2     = GRAPHICS_BACKEND_UNSIGNED_INT_10_10_10_2,
    UNSIGNED_INT_2_10_10_10_REV = GRAPHICS_BACKEND_UNSIGNED_INT_2_10_10_10_REV,
};

#endif //RENDER_ENGINE_TEXTURE_DATA_TYPE_H
