#ifndef OPENGL_STUDY_MATRIX4X4_H
#define OPENGL_STUDY_MATRIX4X4_H

#include "../quaternion/quaternion.h"
#include "../vector3/vector3.h"
#include "string"

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

    static Matrix4x4 Zero();

    static Matrix4x4 Identity();

    static Matrix4x4 Translation(Vector3 _translation);

    static Matrix4x4 Rotation(Quaternion _quaternion);

    static Matrix4x4 Scale(Vector3 _scale);

    static Matrix4x4 TRS(Vector3 _translation, Quaternion _rotation, Vector3 _scale);

    [[nodiscard]] float GetElement(int _column, int _row) const;

    void SetElement(int _column, int _row, float _value);

    Matrix4x4 operator*(const Matrix4x4 &_matrix) const;

    Matrix4x4 Invert() const;

    Matrix4x4 Transpose() const;

    [[nodiscard]] std::string ToString() const;
};

#endif //OPENGL_STUDY_MATRIX4X4_H
