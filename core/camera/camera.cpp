#include "camera.h"
#include "graphics/graphics.h"
#include "gameObject/gameObject.h"

#include <memory>

std::shared_ptr<Camera> Camera::Create(const nlohmann::json& componentData)
{
    float fov = 80;
    float nearClipPlane = 0.01;
    float farClipPlane = 100;

    if (componentData.contains("Fov"))
        componentData.at("Fov").get_to(fov);
    if (componentData.contains("NearClip"))
        componentData.at("NearClip").get_to(nearClipPlane);
    if (componentData.contains("FarClip"))
        componentData.at("FarClip").get_to(farClipPlane);

    std::shared_ptr<Camera> camera = std::make_shared<Camera>(fov, nearClipPlane, farClipPlane);
    Camera::Current = camera;
    return camera;
}

DEFINE_COMPONENT_DEFAULT_ASYNC_CONSTRUCTOR(Camera)

std::shared_ptr<Camera> Camera::Current = nullptr;

Camera::Camera(float fov, float nearClipPlane, float farClipPlane) :
    m_Fov(fov),
    m_NearClipPlane(nearClipPlane),
    m_FarClipPlane(farClipPlane),
    m_ProjectionMatrix(Matrix4x4::Identity()),
    m_ScreenWidth(0),
    m_ScreenHeight(0)
{
}
