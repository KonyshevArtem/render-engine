#ifndef OPENGL_STUDY_CAMERA_FLY_CONTROLLER_H
#define OPENGL_STUDY_CAMERA_FLY_CONTROLLER_H

#include "../../math/vector2/vector2.h"

class CameraFlyController
{
public:
    void Update();

private:
    Vector2 m_CameraEulerAngles;
    
#ifdef OPENGL_STUDY_WINDOWS
    const float CAMERA_ROT_SPEED  = 0.035f;
    const float CAMERA_MOVE_SPEED = 0.025f;
#elif OPENGL_STUDY_MACOS
    const float CAMERA_ROT_SPEED  = 0.005f;
    const float CAMERA_MOVE_SPEED = 0.1f;
#endif
};

#endif //OPENGL_STUDY_CAMERA_FLY_CONTROLLER_H
