#ifndef OPENGL_STUDY_CAMERA_FLY_CONTROLLER_H
#define OPENGL_STUDY_CAMERA_FLY_CONTROLLER_H

#include "vector2/vector2.h"

class CameraFlyController
{
public:
    void Update();

private:
    Vector2 m_CameraEulerAngles;

    static constexpr float CAMERA_ROT_SPEED  = 10.0f;
    static constexpr float CAMERA_MOVE_SPEED = 15.0f;
};

#endif //OPENGL_STUDY_CAMERA_FLY_CONTROLLER_H
