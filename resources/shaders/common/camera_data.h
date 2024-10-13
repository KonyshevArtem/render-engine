#ifndef CAMERA_DATA
#define CAMERA_DATA

cbuffer CameraData
{
    float4x4   _VPMatrix;
    float3     _CameraPosWS;
    float      _NearClipPlane;
    float3     _CameraFwdWS;
    float      _FarClipPlane;
};

#endif//CAMERA_DATA