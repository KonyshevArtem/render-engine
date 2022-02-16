#include "camera.h"
#include "graphics/graphics.h"

std::unique_ptr<Camera> Camera::Current = nullptr;

Camera::Camera(float _fov, float _nearClipPlane, float _farClipPlane, float _shadowDistance) :
    m_Fov(_fov),
    m_NearClipPlane(_nearClipPlane),
    m_FarClipPlane(_farClipPlane),
    m_ShadowDistance(_shadowDistance),
    m_Position(Vector3()),
    m_Rotation(Quaternion()),
    m_ViewMatrix(Matrix4x4::Identity()),
    m_ProjectionMatrix(Matrix4x4::Identity()),
    m_DirtyView(true),
    m_ScreenWidth(0),
    m_ScreenHeight(0)
{
}

void Camera::Init(float _fov, float _nearClipPlane, float _farClipPlane, float _shadowDistance)
{
    Current = std::unique_ptr<Camera>(new Camera(_fov, _nearClipPlane, _farClipPlane, _shadowDistance));
}

const Matrix4x4 &Camera::GetViewMatrix()
{
    if (m_DirtyView)
    {
        m_DirtyView  = false;
        m_ViewMatrix = Matrix4x4::Rotation(m_Rotation.Inverse()) * Matrix4x4::Translation(-m_Position);
    }

    return m_ViewMatrix;
}

const Matrix4x4 &Camera::GetProjectionMatrix()
{
    int width  = Graphics::GetScreenWidth();
    int height = Graphics::GetScreenHeight();
    if (width != m_ScreenWidth || height != m_ScreenHeight)
    {
        float aspect       = static_cast<float>(width) / static_cast<float>(height);
        m_ProjectionMatrix = Matrix4x4::Perspective(m_Fov, aspect, m_NearClipPlane, m_FarClipPlane);
    }

    return m_ProjectionMatrix;
}

void Camera::SetPosition(const Vector3 &_position)
{
    m_Position  = _position;
    m_DirtyView = true;
}

void Camera::SetRotation(const Quaternion &_rotation)
{
    m_Rotation  = _rotation;
    m_DirtyView = true;
}
