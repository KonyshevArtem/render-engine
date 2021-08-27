#include "camera_fly_controller.h"
#include "../../core/camera/camera.h"
#include "../../core/input/input.h"
#include "../../core/time/time.h" // NOLINT(modernize-deprecated-headers)

void CameraFlyController::Update()
{
    cameraEulerAngles = cameraEulerAngles + Input::GetMouseDelta() * cameraRotSpeed * Time::DeltaTime;
    if (cameraEulerAngles.x < 0)
        cameraEulerAngles.x = 360;
    if (cameraEulerAngles.x > 360)
        cameraEulerAngles.x = 0;
    if (cameraEulerAngles.y < 0)
        cameraEulerAngles.y = 360;
    if (cameraEulerAngles.y > 360)
        cameraEulerAngles.y = 0;

    Quaternion xRot           = Quaternion::AngleAxis(cameraEulerAngles.y, Vector3(1, 0, 0));
    Quaternion yRot           = Quaternion::AngleAxis(cameraEulerAngles.x, Vector3(0, 1, 0));
    Quaternion cameraRotation = yRot * xRot;

    Vector3 cameraPosition = Camera::Current->Position;

    Vector3 cameraFwd   = cameraRotation * Vector3(0, 0, -1);
    Vector3 cameraRight = cameraRotation * Vector3(1, 0, 0);
    if (Input::IsKeyDown('w'))
        cameraPosition = cameraPosition + cameraFwd * cameraMoveSpeed;
    if (Input::IsKeyDown('s'))
        cameraPosition = cameraPosition - cameraFwd * cameraMoveSpeed;
    if (Input::IsKeyDown('d'))
        cameraPosition = cameraPosition + cameraRight * cameraMoveSpeed;
    if (Input::IsKeyDown('a'))
        cameraPosition = cameraPosition - cameraRight * cameraMoveSpeed;

    Camera::Current->Position = cameraPosition;
    Camera::Current->Rotation = cameraRotation;
}
