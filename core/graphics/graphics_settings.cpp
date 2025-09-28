#include "graphics_settings.h"
#include "global_constants.h"

#include <array>

#define DEFINE_GRAPHICS_SETTING(type, name, defaultValue)       type s_##name = defaultValue; \
                                                                void Set##name(type value) { s_##name = value; } \
                                                                type Get##name() { return s_##name; }

#define DEFINE_GRAPHICS_SETTING_ARRAY(type, count, name, ...)  std::array<type, count> s_##name = {__VA_ARGS__}; \
                                                               void Set##name(type value, int index) { s_##name[index] = value; } \
                                                               float Get##name(int index) { return s_##name[index]; }

namespace GraphicsSettings
{
    DEFINE_GRAPHICS_SETTING(Vector3, AmbientLightColor, Vector3(1, 1, 1))
    DEFINE_GRAPHICS_SETTING(float, AmbientLightIntensity, 0.1f)

    DEFINE_GRAPHICS_SETTING(Vector3, SunLightColor, Vector3(1, 1, 1))
    DEFINE_GRAPHICS_SETTING(float, SunLightIntensity, 5)

    DEFINE_GRAPHICS_SETTING(float, Gamma, 2.2f)
    DEFINE_GRAPHICS_SETTING(float, Exposure, 1.0f)
    DEFINE_GRAPHICS_SETTING(TonemappingMode, TonemappingMode, TonemappingMode::ACES_APPROXIMATE)

    DEFINE_GRAPHICS_SETTING(float, ShadowDistance, 50)
    DEFINE_GRAPHICS_SETTING_ARRAY(float, GlobalConstants::ShadowCascadeCount, ShadowCascadeBounds, 0.05f, 0.2f, 0.5f, 1.0f)
}