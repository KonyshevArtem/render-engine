#ifndef OPENGL_STUDY_SHADOWS_DATA_H
#define OPENGL_STUDY_SHADOWS_DATA_H

#include "graphics/graphics.h"
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
    Matrix4x4 SpotLightsViewProjMatrices[Graphics::MAX_SPOT_LIGHT_SOURCES]{};
    PointLightShadowData PointLightShadows[Graphics::MAX_POINT_LIGHT_SOURCES]{};
};

#endif //OPENGL_STUDY_SHADOWS_DATA_H
