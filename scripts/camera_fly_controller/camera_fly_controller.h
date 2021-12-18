#ifndef OPENGL_STUDY_CAMERA_FLY_CONTROLLER_H
#define OPENGL_STUDY_CAMERA_FLY_CONTROLLER_H

#include "../../math/vector2/vector2.h"

class CameraFlyController
{
public:
    void Update();

private:
    Vector2     m_CameraEulerAngles;
    const float CAMERA_ROT_SPEED  = 0.005f;
    const float CAMERA_MOVE_SPEED = 0.1f;
};

#endif //OPENGL_STUDY_CAMERA_FLY_CONTROLLER_H
