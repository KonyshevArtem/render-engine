#ifndef RENDER_ENGINE_CAMERA_H
#define RENDER_ENGINE_CAMERA_H

#include "matrix4x4/matrix4x4.h"
#include "quaternion/quaternion.h"
#include "vector3/vector3.h"
#include "component/component.h"
#include <memory>

class Camera : public Component
{
public:
    DECLARE_COMPONENT_CONSTRUCTORS(Camera)

    Camera(float _fov, float _nearClipPlane, float _farClipPlane);

    static std::shared_ptr<Camera> Current;

    inline float GetNearClipPlane() const
    {
        return m_NearClipPlane;
    }

    inline float GetFarClipPlane() const
    {
        return m_FarClipPlane;
    }

    inline float GetFov() const
    {
        return m_Fov;
    }

    ~Camera() = default;

private:
    Camera(const Camera &) = delete;
    Camera(Camera &&)      = delete;

    Camera &operator=(const Camera &) = delete;
    Camera &operator=(Camera &&)      = delete;

    float      m_Fov            = 0;
    float      m_NearClipPlane  = 0;
    float      m_FarClipPlane   = 0;
    Matrix4x4  m_ProjectionMatrix;
    int        m_ScreenWidth;
    int        m_ScreenHeight;
};

#endif //RENDER_ENGINE_CAMERA_H
