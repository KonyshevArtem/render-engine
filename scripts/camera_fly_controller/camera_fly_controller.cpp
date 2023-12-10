#include "camera_fly_controller.h"
#include "camera/camera.h"
#include "input/input.h"
#include "time/time.h" // NOLINT(modernize-deprecated-headers)

void CameraFlyController::Update()
{
    if (Input::GetKey('e'))
        m_CameraEulerAngles = m_CameraEulerAngles + Input::GetMouseDelta() * (CAMERA_ROT_SPEED * Time::GetDeltaTime());
    if (m_CameraEulerAngles.x < 0)
        m_CameraEulerAngles.x = 360;
    if (m_CameraEulerAngles.x > 360)
        m_CameraEulerAngles.x = 0;
    if (m_CameraEulerAngles.y < 0)
        m_CameraEulerAngles.y = 360;
    if (m_CameraEulerAngles.y > 360)
        m_CameraEulerAngles.y = 0;

    auto xRot           = Quaternion::AngleAxis(m_CameraEulerAngles.y, Vector3(-1, 0, 0));
    auto yRot           = Quaternion::AngleAxis(m_CameraEulerAngles.x, Vector3(0, -1, 0));
    auto cameraRotation = yRot * xRot;

    auto cameraPosition = Camera::Current->GetPosition();

    auto cameraFwd   = cameraRotation * Vector3(0, 0, 1);
    auto cameraRight = cameraRotation * Vector3(1, 0, 0);
    if (Input::GetKey('w'))
        cameraPosition = cameraPosition + cameraFwd * CAMERA_MOVE_SPEED * Time::GetDeltaTime();
    if (Input::GetKey('s'))
        cameraPosition = cameraPosition - cameraFwd * CAMERA_MOVE_SPEED * Time::GetDeltaTime();
    if (Input::GetKey('d'))
        cameraPosition = cameraPosition + cameraRight * CAMERA_MOVE_SPEED * Time::GetDeltaTime();
    if (Input::GetKey('a'))
        cameraPosition = cameraPosition - cameraRight * CAMERA_MOVE_SPEED * Time::GetDeltaTime();

    Camera::Current->SetPosition(cameraPosition);
    Camera::Current->SetRotation(cameraRotation);
}
