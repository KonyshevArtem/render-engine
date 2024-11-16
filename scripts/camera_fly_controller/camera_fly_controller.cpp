#include "camera_fly_controller.h"
#include "camera/camera.h"
#include "graphics/graphics.h"
#include "input/input.h"
#include "time/time.h" // NOLINT(modernize-deprecated-headers)

#include <algorithm>

constexpr float CAMERA_ROT_SPEED  = 10.0f;
constexpr float CAMERA_MOVE_SPEED = 15.0f;
constexpr float TOUCH_MOVE_MAX_DELTA = 100.0f;
constexpr float TOUCH_ROTATE_MIN_DELTA = 5.0f;

void CameraFlyController::Update()
{
    float cameraRotationDelta = CAMERA_ROT_SPEED * Time::GetDeltaTime();
    if (Input::GetMouseButton(Input::MouseButton::RIGHT))
        m_CameraEulerAngles += Input::GetMouseDelta() * cameraRotationDelta;

    if (m_HasRotateTouch)
    {
        Input::Touch touch;
        Input::GetTouch(m_RotateTouchId, touch);
        if (touch.Delta.Length() > TOUCH_ROTATE_MIN_DELTA)
            m_CameraEulerAngles -= touch.Delta * cameraRotationDelta;
    }

    if (m_CameraEulerAngles.x < 0)
        m_CameraEulerAngles.x = 360;
    if (m_CameraEulerAngles.x > 360)
        m_CameraEulerAngles.x = 0;
    if (m_CameraEulerAngles.y < 0)
        m_CameraEulerAngles.y = 360;
    if (m_CameraEulerAngles.y > 360)
        m_CameraEulerAngles.y = 0;

    const Quaternion xRot = Quaternion::AngleAxis(m_CameraEulerAngles.y, Vector3(-1, 0, 0));
    const Quaternion yRot = Quaternion::AngleAxis(m_CameraEulerAngles.x, Vector3(0, -1, 0));
    const Quaternion cameraRotation = yRot * xRot;

    Vector3 cameraPosition = Camera::Current->GetPosition();

    float cameraMoveDelta = CAMERA_MOVE_SPEED * Time::GetDeltaTime();
    const Vector3 cameraFwd = cameraRotation * Vector3(0, 0, 1) * cameraMoveDelta;
    const Vector3 cameraRight = cameraRotation * Vector3(1, 0, 0) * cameraMoveDelta;
    const Vector3 cameraUp = cameraRotation * Vector3(0, 1, 0) * cameraMoveDelta;
    if (Input::GetKey('W'))
        cameraPosition += cameraFwd;
    if (Input::GetKey('S'))
        cameraPosition -= cameraFwd;
    if (Input::GetKey('D'))
        cameraPosition += cameraRight;
    if (Input::GetKey('A'))
        cameraPosition -= cameraRight;
    if (Input::GetKey('E'))
        cameraPosition += cameraUp;
    if (Input::GetKey('Q'))
        cameraPosition -= cameraUp;

    if (m_HasMoveTouch)
    {
        Input::Touch touch;
        Input::GetTouch(m_MoveTouchId, touch);
        Vector2 delta = (touch.Position - m_MoveTouchStartPos) / TOUCH_MOVE_MAX_DELTA;
        delta.x = std::clamp(delta.x, -1.0f, 1.0f);
        delta.y = std::clamp(delta.y, -1.0f, 1.0f);
        cameraPosition = cameraPosition + cameraFwd * -delta.y;
        cameraPosition = cameraPosition + cameraRight * delta.x;
    }

    UpdateTouchInputs();

    Camera::Current->SetPosition(cameraPosition);
    Camera::Current->SetRotation(cameraRotation);
}

void CameraFlyController::UpdateTouchInputs()
{
    for (const Input::Touch& touch: Input::GetTouches())
    {
        if (touch.State == Input::TouchState::DOWN)
        {
            float halfScreenWidth = 0.5f * Graphics::GetScreenWidth();
            if (touch.Position.x < halfScreenWidth && !m_HasMoveTouch)
            {
                m_HasMoveTouch = true;
                m_MoveTouchId = touch.Id;
                m_MoveTouchStartPos = touch.Position;
            }

            if (touch.Position.x > halfScreenWidth && !m_HasRotateTouch)
            {
                m_HasRotateTouch = true;
                m_RotateTouchId = touch.Id;
            }
        }
        else if (touch.State == Input::TouchState::UP)
        {
            if (m_HasMoveTouch && touch.Id == m_MoveTouchId)
            {
                m_HasMoveTouch = false;
            }
            if (m_HasRotateTouch && touch.Id == m_RotateTouchId)
            {
                m_HasRotateTouch = false;
            }
        }
    }
}
