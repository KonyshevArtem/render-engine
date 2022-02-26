#ifndef OPENGL_STUDY_MATRIX4X4_H
#define OPENGL_STUDY_MATRIX4X4_H

#include <string>

struct Quaternion;
struct Vector3;
struct Vector4;

struct [[nodiscard]] Matrix4x4
{
public:
    float m00;
    float m01;
    float m02;
    float m03;
    float m10;
    float m11;
    float m12;
    float m13;
    float m20;
    float m21;
    float m22;
    float m23;
    float m30;
    float m31;
    float m32;
    float m33;

    static const Matrix4x4 &Zero();

    static const Matrix4x4 &Identity();

    static Matrix4x4 Translation(const Vector3 &_translation);

    static Matrix4x4 Rotation(const Quaternion &_quaternion);

    static Matrix4x4 Scale(const Vector3 &_scale);

    static Matrix4x4 TRS(const Vector3 &_translation, const Quaternion &_rotation, const Vector3 &_scale);

    static Matrix4x4 Perspective(float _fov, float _aspect, float _nearZ, float _farZ);

    static Matrix4x4 Orthographic(float _left, float _right, float _bottom, float _top, float _near, float _far);

    [[nodiscard]] float GetElement(int _column, int _row) const;

    void SetElement(int _column, int _row, float _value);

    Matrix4x4 operator*(const Matrix4x4 &_matrix) const;

    Vector4 operator*(const Vector4 &_matrix) const;

    Matrix4x4 Invert() const;

    Matrix4x4 Transpose() const;

    Vector3 GetPosition() const;
};

#endif //OPENGL_STUDY_MATRIX4X4_H
