#ifndef GLOBAL_DEFINES_H
#define GLOBAL_DEFINES_H

// Keep in-sync with core/global_constants.h

#define SHADOW_CASCADE_COUNT       4
#define MAX_POINT_LIGHT_SOURCES    3
#define MAX_SPOT_LIGHT_SOURCES     3

#define PI 3.14159265359

#if defined(OPENGL_SHADER) || defined(GLES_SHADER)
    #define SCREEN_UV_UPSIDE_DOWN 0
#elif defined(METAL_SHADER) || defined(DX12_SHADER)
    #define SCREEN_UV_UPSIDE_DOWN 1
#endif

#define REFLECTION_CUBE                 t4
#define REFLECTION_CUBE_SAMPLER         s4
#define DIRECTIONAL_SHADOW_MAP          t5
#define DIRECTIONAL_SHADOW_MAP_SAMPLER  s5
#define SPOTLIGHT_SHADOW_MAP            t6
#define SPOTLIGHT_SHADOW_MAP_SAMPLER    s6
#define POINTLIGHT_SHADOW_MAP           t7
#define POINTLIGHT_SHADOW_MAP_SAMPLER   s7

#define MATRICES_DATA   b4
#define LIGHTING_DATA   b5
#define CAMERA_DATA     b6
#define SHADOW_DATA     b7

#define INSTANCING_MATRICES_ENTRIES_DATA   t6, space1
#define TRANSFORM_MATRICES_DATA            t7, space1

#define RT_SCENE                t0, space3
#define RT_PER_INSTANCE_DATA    t5, space1

#define BINDLESS_RESOURCES t0, space2

#endif //GLOBAL_DEFINES_H
