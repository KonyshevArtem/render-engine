#ifndef RENDER_ENGINE_SHADOWS_DATA_H
#define RENDER_ENGINE_SHADOWS_DATA_H

#include "global_constants.h"
#include "vector4/vector4.h"
#include "matrix4x4/matrix4x4.h"

// Keep this in-sync with shaders/common/shadows.cg
struct ShadowsData
{
    struct PointLightShadowData
    {
        Matrix4x4 ViewProjMatrices[6]{};
        Vector4 Position{};
    };

    Matrix4x4 DirectionalLightViewProjMatrix{};
    Matrix4x4 SpotLightsViewProjMatrices[GlobalConstants::MaxSpotLightSources]{};
    PointLightShadowData PointLightShadows[GlobalConstants::MaxPointLightSources]{};
};

#endif //RENDER_ENGINE_SHADOWS_DATA_H
