#ifndef RENDER_ENGINE_PER_DRAW_DATA_H
#define RENDER_ENGINE_PER_DRAW_DATA_H

#include "matrix4x4/matrix4x4.h"

// Keep in-sync with shaders/common/per_draw_data.cg
struct PerDrawData
{
    Matrix4x4 ModelMatrix;
    Matrix4x4 ModelNormalMatrix;
};

#endif //RENDER_ENGINE_PER_DRAW_DATA_H
