#ifndef OPENGL_STUDY_CAMERA_H
#define OPENGL_STUDY_CAMERA_H

#include "../../math/matrix4x4/matrix4x4.h"
#include "../../math/quaternion/quaternion.h"
#include "../../math/vector3/vector3.h"
#include <memory>

using namespace std;

class Camera
{
public:
    static void Init(float _fov, float _nearClipPlane, float _farClipPlane);

    static unique_ptr<Camera> Current;

    const Matrix4x4 &GetViewMatrix();
    const Matrix4x4 &GetProjectionMatrix();
    void             SetPosition(const Vector3 &_position);
    void             SetRotation(const Quaternion &_rotation);

    inline const Vector3 &GetPosition() const
    {
        return m_Position;
    }

    inline const Quaternion &GetRotation() const
    {
        return m_Rotation;
    }

    inline float GetNearClipPlane() const
    {
        return m_NearClipPlane;
    }

    inline float GetFarClipPlane() const
    {
        return m_FarClipPlane;
    }

    ~Camera() = default;

private:
    Camera(float _fov, float _nearClipPlane, float _farClipPlane);
    Camera(const Camera &) = delete;
    Camera(Camera &&)      = delete;

    Camera &operator=(const Camera &) = delete;
    Camera &operator=(Camera &&)      = delete;

    float      m_Fov           = 0;
    float      m_NearClipPlane = 0;
    float      m_FarClipPlane  = 0;
    Vector3    m_Position;
    Quaternion m_Rotation;
    Matrix4x4  m_ViewMatrix;
    Matrix4x4  m_ProjectionMatrix;
    bool       m_DirtyView;
    int        m_ScreenWidth;
    int        m_ScreenHeight;
};

#endif //OPENGL_STUDY_CAMERA_H
