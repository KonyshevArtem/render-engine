#ifndef RENDER_ENGINE_SHADOWS_DATA_H
#define RENDER_ENGINE_SHADOWS_DATA_H

#include "global_constants.h"
#include "vector4/vector4.h"
#include "matrix4x4/matrix4x4.h"

// Keep this in-sync with shaders/common/shadows.h
struct ShadowsData
{
    struct SpotLightShadowData
    {
        Matrix4x4 ViewProjMatrix{};

        Vector3 Padding0{};
        uint32_t ShadowAtlasSlot{};
    };

    struct PointLightShadowData
    {
        Matrix4x4 ViewProjMatrices[6]{};
        Vector4 Position{};

        uint32_t ShadowAtlasSlots[8]{};
    };

    Matrix4x4 DirectionalLightViewProjMatrix[GlobalConstants::ShadowCascadeCount]{};
    SpotLightShadowData SpotLightShadows[GlobalConstants::MaxSpotLightSources]{};
    PointLightShadowData PointLightShadows[GlobalConstants::MaxPointLightSources]{};
};

#endif //RENDER_ENGINE_SHADOWS_DATA_H
