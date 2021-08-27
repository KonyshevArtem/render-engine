#ifndef OPENGL_STUDY_CAMERA_H
#define OPENGL_STUDY_CAMERA_H

#include "../../math/matrix4x4/matrix4x4.h"
#include "../../math/quaternion/quaternion.h"
#include "../../math/vector3/vector3.h"
#include "memory"

using namespace std;

class Camera
{
public:
    static void Init(float fov, float zNear, float zFar);
    Matrix4x4   GetViewMatrix();
    Matrix4x4   GetProjectionMatrix();

    inline static unique_ptr<Camera> Current  = nullptr;
    Vector3                          Position = Vector3::Zero();
    Quaternion                       Rotation = Quaternion();

private:
    float Fov;
    float zNear;
    float zFar;
};

#endif //OPENGL_STUDY_CAMERA_H
