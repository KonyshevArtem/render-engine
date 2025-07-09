#include "camera.h"
#include "graphics/graphics.h"
#include "gameObject/gameObject.h"

#include <memory>

REGISTER_COMPONENT(Camera)

std::shared_ptr<Camera> Camera::Create(const nlohmann::json& componentData)
{
    float fov = 80;
    float nearClipPlane = 0.01;
    float farClipPlane = 100;
    float shadowDistance = 100;

    if (componentData.contains("Fov"))
        componentData.at("Fov").get_to(fov);
    if (componentData.contains("NearClip"))
        componentData.at("NearClip").get_to(nearClipPlane);
    if (componentData.contains("FarClip"))
        componentData.at("FarClip").get_to(farClipPlane);
    if (componentData.contains("ShadowDistance"))
        componentData.at("ShadowDistance").get_to(shadowDistance);

    std::shared_ptr<Camera> camera = std::make_shared<Camera>(fov, nearClipPlane, farClipPlane, shadowDistance);
    Camera::Current = camera;
    return camera;
}

std::shared_ptr<Camera> Camera::Current = nullptr;

Camera::Camera(float fov, float nearClipPlane, float farClipPlane, float shadowDistance) :
    m_Fov(fov),
    m_NearClipPlane(nearClipPlane),
    m_FarClipPlane(farClipPlane),
    m_ShadowDistance(shadowDistance),
    m_ProjectionMatrix(Matrix4x4::Identity()),
    m_ScreenWidth(0),
    m_ScreenHeight(0)
{
}

const Matrix4x4 &Camera::GetViewMatrix()
{
    return m_GameObject.lock()->GetWorldToLocalMatrix();
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
