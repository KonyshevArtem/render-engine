#ifndef RENDER_ENGINE_SHADER_STRUCTS_H
#define RENDER_ENGINE_SHADER_STRUCTS_H

#include "enums/blend_factor.h"
#include "enums/cull_face.h"
#include "enums/cull_face_orientation.h"

struct BlendInfo
{
    bool Enabled = false;
    BlendFactor SourceFactor = BlendFactor::ONE;
    BlendFactor DestinationFactor = BlendFactor::ONE;
};

#endif //RENDER_ENGINE_SHADER_STRUCTS_H
