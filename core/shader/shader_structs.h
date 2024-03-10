#ifndef RENDER_ENGINE_SHADER_STRUCTS_H
#define RENDER_ENGINE_SHADER_STRUCTS_H

#include "enums/depth_function.h"
#include "enums/blend_factor.h"
#include "enums/cull_face.h"

struct BlendInfo
{
    bool Enabled = false;
    BlendFactor SourceFactor = BlendFactor::ONE;
    BlendFactor DestinationFactor = BlendFactor::ONE;
};

struct CullInfo
{
    bool Enabled = true;
    CullFace Face = CullFace::BACK;
};

struct DepthInfo
{
    bool WriteDepth = true;
    DepthFunction DepthFunction = DepthFunction::LEQUAL;
};

#endif //RENDER_ENGINE_SHADER_STRUCTS_H
