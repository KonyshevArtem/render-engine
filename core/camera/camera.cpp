#include "camera.h"
#include "../graphics/graphics.h"

void Camera::Init(float _fov, float _nearZ, float _farZ)
{
    Current          = make_unique<Camera>();
    Current->m_Fov   = _fov;
    Current->m_NearZ = _nearZ;
    Current->m_FarZ  = _farZ;
}

Matrix4x4 Camera::GetViewMatrix() const
{
    return Matrix4x4::Rotation(Rotation.Inverse()) * Matrix4x4::Translation(-Position);
}

Matrix4x4 Camera::GetProjectionMatrix() const
{
    Matrix4x4 perspectiveMatrix = Matrix4x4::Zero();

    float aspect = (float) Graphics::ScreenWidth / (float) Graphics::ScreenHeight;
    float top    = m_NearZ * ((float) M_PI / 180 * m_Fov / 2);
    float bottom = -top;
    float right  = aspect * top;
    float left   = -right;

    perspectiveMatrix.m00 = 2 * m_NearZ / (right - left);
    perspectiveMatrix.m11 = 2 * m_NearZ / (top - bottom);
    perspectiveMatrix.m20 = (right + left) / (right - left);
    perspectiveMatrix.m21 = (top + bottom) / (top - bottom);
    perspectiveMatrix.m22 = -(m_FarZ + m_NearZ) / (m_FarZ - m_NearZ);
    perspectiveMatrix.m23 = -1;
    perspectiveMatrix.m32 = -2 * m_FarZ * m_NearZ / (m_FarZ - m_NearZ);

    return perspectiveMatrix;
}
