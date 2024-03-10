#ifndef RENDER_ENGINE_CAMERA_DATA_H
#define RENDER_ENGINE_CAMERA_DATA_H

#include "vector3/vector3.h"
#include "matrix4x4/matrix4x4.h"

// Keep in-sync with shaders/common/camera_data.cg
struct CameraData
{
    Matrix4x4 ViewProjectionMatrix{};
    Vector3 CameraPosition{};
    float NearClipPlane = 0;
    Vector3 CameraDirection{};
    float FarClipPlane = 0;
};

#endif //RENDER_ENGINE_CAMERA_DATA_H
