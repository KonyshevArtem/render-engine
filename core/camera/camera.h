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
    static void Init(float _fov, float _nearZ, float _farZ);
    Matrix4x4   GetViewMatrix() const;
    Matrix4x4   GetProjectionMatrix() const;

    inline static unique_ptr<Camera> Current  = nullptr;
    Vector3                          Position = Vector3();
    Quaternion                       Rotation = Quaternion();

private:
    float m_Fov   = 0;
    float m_NearZ = 0;
    float m_FarZ  = 0;
};

#endif //OPENGL_STUDY_CAMERA_H
