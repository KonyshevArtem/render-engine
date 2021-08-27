#ifndef OPENGL_STUDY_CAMERA_FLY_CONTROLLER_H
#define OPENGL_STUDY_CAMERA_FLY_CONTROLLER_H

#include "../../math/vector3/vector3.h"

class CameraFlyController
{
public:
    void Update();

private:
    Vector3     cameraEulerAngles;
    const float cameraRotSpeed  = 0.005f;
    const float cameraMoveSpeed = 0.1f;
};

#endif //OPENGL_STUDY_CAMERA_FLY_CONTROLLER_H
