#ifndef RENDER_ENGINE_GRAPHICS_SETTINGS_H
#define RENDER_ENGINE_GRAPHICS_SETTINGS_H

#include "vector3/vector3.h"

#define DECLARE_GRAPHICS_SETTING(type, name)    void Set##name(type value); \
                                                type Get##name();

#define DECLARE_GRAPHICS_SETTING_ARRAY(type, name)  void Set##name(type value, int index); \
                                                    type Get##name(int index);

namespace GraphicsSettings
{
    enum class TonemappingMode
    {
        NONE                                    = 0,
        REINHARD_SIMPLE                         = 1,
        REINHARD_LUMINANCE                      = 2,
        REINHARD_LUMINANCE_WHITE_PRESERVING     = 3,
        ROMBINDAHOUSE                           = 4,
        FILMIC                                  = 5,
        UNCHARTED2                              = 6,
        ACES                                    = 7,
        ACES_APPROXIMATE                        = 8
    };

    DECLARE_GRAPHICS_SETTING(Vector3, AmbientLightColor)
    DECLARE_GRAPHICS_SETTING(float, AmbientLightIntensity)

    DECLARE_GRAPHICS_SETTING(Vector3, SunLightColor)
    DECLARE_GRAPHICS_SETTING(float, SunLightIntensity)

    DECLARE_GRAPHICS_SETTING(float, Gamma)
    DECLARE_GRAPHICS_SETTING(float, Exposure)
    DECLARE_GRAPHICS_SETTING(TonemappingMode, TonemappingMode)

    DECLARE_GRAPHICS_SETTING(float, ShadowDistance)
    DECLARE_GRAPHICS_SETTING_ARRAY(float, ShadowCascadeBounds)
}

#endif //RENDER_ENGINE_GRAPHICS_SETTINGS_H
