#ifndef CAMERA_DATA
#define CAMERA_DATA

layout(std140) uniform CameraData // 16 bytes (12 bytes round up by 16)
{
    vec3 cameraPosWS; // 12 bytes
};

#endif //CAMERA_DATA