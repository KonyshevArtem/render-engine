#ifndef RENDER_ENGINE_CAMERA_FLY_CONTROLLER_H
#define RENDER_ENGINE_CAMERA_FLY_CONTROLLER_H

#include <cstdint>
#include "vector2/vector2.h"
#include "component/component.h"

class CameraFlyController : public Component
{
public:
    DECLARE_COMPONENT_CONSTRUCTORS(CameraFlyController)

    void Update() override;

    CameraFlyController() = default;
    ~CameraFlyController() = default;

    CameraFlyController(const CameraFlyController &) = delete;
    CameraFlyController(CameraFlyController &&)      = delete;

    CameraFlyController &operator=(const CameraFlyController &) = delete;
    CameraFlyController &operator=(CameraFlyController &&) = delete;

private:
    void UpdateTouchInputs();

    Vector2 m_CameraEulerAngles;

    bool m_HasMoveTouch = false;
    uint64_t m_MoveTouchId = 0;
    Vector2 m_MoveTouchStartPos{};

    bool m_HasRotateTouch = false;
    uint64_t m_RotateTouchId = 0;
};

#endif //RENDER_ENGINE_CAMERA_FLY_CONTROLLER_H
