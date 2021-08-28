#include "camera_fly_controller.h"
#include "../../core/camera/camera.h"
#include "../../core/input/input.h"
#include "../../core/time/time.h" // NOLINT(modernize-deprecated-headers)

void CameraFlyController::Update()
{
    m_CameraEulerAngles = m_CameraEulerAngles + Input::GetMouseDelta() * CAMERA_ROT_SPEED * Time::DeltaTime;
    if (m_CameraEulerAngles.x < 0)
        m_CameraEulerAngles.x = 360;
    if (m_CameraEulerAngles.x > 360)
        m_CameraEulerAngles.x = 0;
    if (m_CameraEulerAngles.y < 0)
        m_CameraEulerAngles.y = 360;
    if (m_CameraEulerAngles.y > 360)
        m_CameraEulerAngles.y = 0;

    Quaternion xRot           = Quaternion::AngleAxis(m_CameraEulerAngles.y, Vector3(1, 0, 0));
    Quaternion yRot           = Quaternion::AngleAxis(m_CameraEulerAngles.x, Vector3(0, 1, 0));
    Quaternion cameraRotation = yRot * xRot;

    Vector3 cameraPosition = Camera::Current->Position;

    Vector3 cameraFwd   = cameraRotation * Vector3(0, 0, -1);
    Vector3 cameraRight = cameraRotation * Vector3(1, 0, 0);
    if (Input::IsKeyDown('w'))
        cameraPosition = cameraPosition + cameraFwd * CAMERA_MOVE_SPEED;
    if (Input::IsKeyDown('s'))
        cameraPosition = cameraPosition - cameraFwd * CAMERA_MOVE_SPEED;
    if (Input::IsKeyDown('d'))
        cameraPosition = cameraPosition + cameraRight * CAMERA_MOVE_SPEED;
    if (Input::IsKeyDown('a'))
        cameraPosition = cameraPosition - cameraRight * CAMERA_MOVE_SPEED;

    Camera::Current->Position = cameraPosition;
    Camera::Current->Rotation = cameraRotation;
}
