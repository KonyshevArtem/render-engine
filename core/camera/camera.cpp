#include "camera.h"
#include "../../math/matrix4x4/matrix4x4.h"
#include "../graphics/graphics.h"

void Camera::Init(float _fov, float _nearClipPlane, float _farClipPlane)
{
    Current                = make_unique<Camera>();
    Current->m_Fov         = _fov;
    Current->NearClipPlane = _nearClipPlane;
    Current->FarClipPlane  = _farClipPlane;
}

Matrix4x4 Camera::GetViewMatrix() const
{
    return Matrix4x4::Rotation(Rotation.Inverse()) * Matrix4x4::Translation(-Position);
}

Matrix4x4 Camera::GetProjectionMatrix() const
{
    float aspect = static_cast<float>(Graphics::ScreenWidth) / static_cast<float>(Graphics::ScreenHeight);
    return Matrix4x4::Perspective(m_Fov, aspect, NearClipPlane, FarClipPlane);
}
