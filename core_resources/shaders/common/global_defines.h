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

#endif //GLOBAL_DEFINES_H
