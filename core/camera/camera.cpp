#include "camera.h"
#include "graphics/graphics.h"

#include <memory>

REGISTER_COMPONENT(Camera)

std::shared_ptr<Component> CameraComponentFactory::CreateComponent(const nlohmann::json& componentData)
{
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    if (componentData.contains("Fov"))
        componentData.at("Fov").get_to(camera->m_Fov);
    if (componentData.contains("NearClip"))
        componentData.at("NearClip").get_to(camera->m_NearClipPlane);
    if (componentData.contains("FarClip"))
        componentData.at("FarClip").get_to(camera->m_FarClipPlane);
    if (componentData.contains("ShadowDistance"))
        componentData.at("ShadowDistance").get_to(camera->m_ShadowDistance);

    Camera::Current = camera;
    return camera;
}

std::shared_ptr<Camera> Camera::Current = nullptr;

Camera::Camera() :
    m_Fov(80),
    m_NearClipPlane(0.01),
    m_FarClipPlane(100),
    m_ShadowDistance(100),
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
    Current = std::make_shared<Camera>();
    Current->m_Fov = _fov;
    Current->m_NearClipPlane = _nearClipPlane;
    Current->m_FarClipPlane = _farClipPlane;
    Current->m_ShadowDistance = _shadowDistance;
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
