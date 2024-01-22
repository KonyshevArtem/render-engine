#ifndef OPENGL_STUDY_TEXTURE_PARAMETER_H
#define OPENGL_STUDY_TEXTURE_PARAMETER_H

#include "graphics_backend.h"

enum class TextureParameter : GRAPHICS_BACKEND_TYPE_ENUM
{
    BASE_LEVEL = GRAPHICS_BACKEND_TEXTURE_BASE_LEVEL,
    MAX_LEVEL  = GRAPHICS_BACKEND_TEXTURE_MAX_LEVEL,

    BORDER_COLOR = GRAPHICS_BACKEND_TEXTURE_BORDER_COLOR,

    COMPARE_FUNC = GRAPHICS_BACKEND_TEXTURE_COMPARE_FUNC,
    COMPARE_MODE = GRAPHICS_BACKEND_TEXTURE_COMPARE_MODE,

    MIN_FILTER = GRAPHICS_BACKEND_TEXTURE_MIN_FILTER,
    MAG_FILTER = GRAPHICS_BACKEND_TEXTURE_MAG_FILTER,

    LOD_BIAS = GRAPHICS_BACKEND_TEXTURE_LOD_BIAS,
    MIN_LOD  = GRAPHICS_BACKEND_TEXTURE_MIN_LOD,
    MAX_LOD  = GRAPHICS_BACKEND_TEXTURE_MAX_LOD,

    SWIZZLE_R    = GRAPHICS_BACKEND_TEXTURE_SWIZZLE_R,
    SWIZZLE_G    = GRAPHICS_BACKEND_TEXTURE_SWIZZLE_G,
    SWIZZLE_B    = GRAPHICS_BACKEND_TEXTURE_SWIZZLE_B,
    SWIZZLE_A    = GRAPHICS_BACKEND_TEXTURE_SWIZZLE_A,
    SWIZZLE_RGBA = GRAPHICS_BACKEND_TEXTURE_SWIZZLE_RGBA,

    WRAP_S = GRAPHICS_BACKEND_TEXTURE_WRAP_S,
    WRAP_T = GRAPHICS_BACKEND_TEXTURE_WRAP_T,
    WRAP_R = GRAPHICS_BACKEND_TEXTURE_WRAP_R,
};

#endif //OPENGL_STUDY_TEXTURE_PARAMETER_H