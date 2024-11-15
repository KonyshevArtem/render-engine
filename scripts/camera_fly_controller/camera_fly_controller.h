#ifndef RENDER_ENGINE_CAMERA_FLY_CONTROLLER_H
#define RENDER_ENGINE_CAMERA_FLY_CONTROLLER_H

#include <cstdint>
#include "vector2/vector2.h"

class CameraFlyController
{
public:
    void Update();

private:
    Vector2 m_CameraEulerAngles;

    bool m_HasMoveTouch = false;
    uint64_t m_MoveTouchId = 0;
    Vector2 m_MoveTouchStartPos{};

    bool m_HasRotateTouch = false;
    uint64_t m_RotateTouchId = 0;
};

#endif //RENDER_ENGINE_CAMERA_FLY_CONTROLLER_H
