#include "camera.h"
#include "../graphics/graphics.h"

void Camera::Init(float fov, float zNear, float zFar)
{
    Current        = make_unique<Camera>();
    Current->Fov   = fov;
    Current->zNear = zNear;
    Current->zFar  = zFar;
}

Matrix4x4 Camera::GetViewMatrix()
{
    return Matrix4x4::Rotation(Rotation.Inverse()) * Matrix4x4::Translation(-Position);
}

Matrix4x4 Camera::GetProjectionMatrix()
{
    Matrix4x4 perspectiveMatrix = Matrix4x4::Zero();

    float aspect = (float) Graphics::ScreenWidth / (float) Graphics::ScreenHeight;
    float top    = zNear * ((float) M_PI / 180 * Fov / 2);
    float bottom = -top;
    float right  = aspect * top;
    float left   = -right;

    perspectiveMatrix.m00 = 2 * zNear / (right - left);
    perspectiveMatrix.m11 = 2 * zNear / (top - bottom);
    perspectiveMatrix.m20 = (right + left) / (right - left);
    perspectiveMatrix.m21 = (top + bottom) / (top - bottom);
    perspectiveMatrix.m22 = -(zFar + zNear) / (zFar - zNear);
    perspectiveMatrix.m23 = -1;
    perspectiveMatrix.m32 = -2 * zFar * zNear / (zFar - zNear);

    return perspectiveMatrix;
}
