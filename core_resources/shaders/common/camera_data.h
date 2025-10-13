#ifndef CAMERA_DATA_H
#define CAMERA_DATA_H

#include "global_defines.h"

cbuffer CameraData : register(CAMERA_DATA)
{
    float4x4   _VPMatrix;
    float3     _CameraPosWS;
    float      _NearClipPlane;
    float3     _CameraFwdWS;
    float      _FarClipPlane;
};

#endif// CAMERA_DATA_H