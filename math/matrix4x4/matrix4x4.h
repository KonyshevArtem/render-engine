#ifndef OPENGL_STUDY_MATRIX4X4_H
#define OPENGL_STUDY_MATRIX4X4_H

#include "../quaternion/quaternion.h"
#include "../vector4/vector4.h"
#include "string"

struct Matrix4x4
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

    static Matrix4x4 Translation(Vector4 translation);

    static Matrix4x4 Rotation(Quaternion quaternion);

    static Matrix4x4 Scale(Vector4 scale);

    static Matrix4x4 TRS(Vector4 translation, Quaternion rotation, Vector4 scale);

    float GetElement(int column, int row) const;

    void SetElement(int column, int row, float value);

    Matrix4x4 operator*(const Matrix4x4 &matrix) const;

    Matrix4x4 Invert() const;

    Matrix4x4 Transpose() const;

    std::string ToString() const;
};

#endif//OPENGL_STUDY_MATRIX4X4_H
