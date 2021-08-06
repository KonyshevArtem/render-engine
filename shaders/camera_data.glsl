#ifndef CAMERA_DATA
#define CAMERA_DATA

layout(std140) uniform CameraData // 144 bytes
{
    mat4 _ProjMatrix;  // 0   64
    mat4 _ViewMatrix;  // 64  128
    vec3 _CameraPosWS; // 128 144
};

#endif //CAMERA_DATA