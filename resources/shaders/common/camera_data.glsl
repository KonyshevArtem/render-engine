#ifndef CAMERA_DATA
#define CAMERA_DATA

layout(std140) uniform CameraData
{
    mat4 _ProjMatrix;
    mat4 _ViewMatrix;
    vec3 _CameraPosWS;
    float _NearClipPlane;
    float _FarClipPlane;
};

#endif//CAMERA_DATA