#ifndef CAMERA_DATA
#define CAMERA_DATA

#include "graphics_backend_macros.h"

struct CameraDataStruct
{
    float4x4        _VPMatrix;
    packed_float3   _CameraPosWS;
    float           _NearClipPlane;
    packed_float3   _CameraFwdWS;
    float           _FarClipPlane;
};

#endif//CAMERA_DATA