#include "matrix4x4.h"
#include "../quaternion/quaternion.h"
#include "../vector3/vector3.h"
#include "../vector4/vector4.h"

const Matrix4x4 &Matrix4x4::Zero()
{
    static const Matrix4x4 zero {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    return zero;
}

const Matrix4x4 &Matrix4x4::Identity()
{
    static const Matrix4x4 identity {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    return identity;
}

Matrix4x4 Matrix4x4::Translation(const Vector3 &_translation)
{
    auto translationMatrix = Matrix4x4::Identity();
    translationMatrix.m30  = _translation.x;
    translationMatrix.m31  = _translation.y;
    translationMatrix.m32  = _translation.z;
    return translationMatrix;
}

Matrix4x4 Matrix4x4::Rotation(const Quaternion &_quaternion)
{
    auto  result = Matrix4x4::Identity();
    float x      = _quaternion.x;
    float y      = _quaternion.y;
    float z      = _quaternion.z;
    float w      = _quaternion.w;

    result.m00 = 1 - 2 * y * y - 2 * z * z;
    result.m01 = 2 * x * y + 2 * z * w;
    result.m02 = 2 * x * z - 2 * y * w;

    result.m10 = 2 * x * y - 2 * z * w;
    result.m11 = 1 - 2 * x * x - 2 * z * z;
    result.m12 = 2 * y * z + 2 * x * w;

    result.m20 = 2 * x * z + 2 * y * w;
    result.m21 = 2 * y * z - 2 * x * w;
    result.m22 = 1 - 2 * x * x - 2 * y * y;

    return result;
}

Matrix4x4 Matrix4x4::Scale(const Vector3 &_scale)
{
    auto result = Matrix4x4::Identity();
    result.m00  = _scale.x;
    result.m11  = _scale.y;
    result.m22  = _scale.z;
    return result;
}

Matrix4x4 Matrix4x4::TRS(const Vector3 &_translation, const Quaternion &_rotation, const Vector3 &_scale)
{
    return Translation(_translation) * Rotation(_rotation) * Scale(_scale);
}

float Matrix4x4::GetElement(int _column, int _row) const
{
    const auto *floatPtr = reinterpret_cast<const float *>(this);
    return *(floatPtr + 4 * _column + _row);
}

void Matrix4x4::SetElement(int _column, int _row, float _value)
{
    auto *floatPtr                   = (float *) this;
    *(floatPtr + 4 * _column + _row) = _value;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &_matrix) const
{
    auto result = Matrix4x4::Zero();
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float sum = 0;
            for (int k = 0; k < 4; ++k)
                sum += GetElement(k, j) * _matrix.GetElement(i, k);
            result.SetElement(i, j, sum);
        }
    }
    return result;
}

Vector4 Matrix4x4::operator*(const Vector4 &_vector) const
{
    return {
            m00 * _vector.x + m10 * _vector.y + m20 * _vector.z + m30 * _vector.w,
            m01 * _vector.x + m11 * _vector.y + m21 * _vector.z + m31 * _vector.w,
            m02 * _vector.x + m12 * _vector.y + m22 * _vector.z + m32 * _vector.w,
            m03 * _vector.x + m13 * _vector.y + m23 * _vector.z + m33 * _vector.w,
    };
}

std::string Matrix4x4::ToString() const
{
    std::string str;
    for (int i = 0; i < 4; ++i)
    {
        str.append(std::to_string(GetElement(0, i)));
        str.append(", ");
        str.append(std::to_string(GetElement(1, i)));
        str.append(", ");
        str.append(std::to_string(GetElement(2, i)));
        str.append(", ");
        str.append(std::to_string(GetElement(3, i)));
        str.append("\n");
    }
    return str;
}

Matrix4x4 Matrix4x4::Invert() const
{
    auto A2323 = m22 * m33 - m23 * m32;
    auto A1323 = m21 * m33 - m23 * m31;
    auto A1223 = m21 * m32 - m22 * m31;
    auto A0323 = m20 * m33 - m23 * m30;
    auto A0223 = m20 * m32 - m22 * m30;
    auto A0123 = m20 * m31 - m21 * m30;
    auto A2313 = m12 * m33 - m13 * m32;
    auto A1313 = m11 * m33 - m13 * m31;
    auto A1213 = m11 * m32 - m12 * m31;
    auto A2312 = m12 * m23 - m13 * m22;
    auto A1312 = m11 * m23 - m13 * m21;
    auto A1212 = m11 * m22 - m12 * m21;
    auto A0313 = m10 * m33 - m13 * m30;
    auto A0213 = m10 * m32 - m12 * m30;
    auto A0312 = m10 * m23 - m13 * m20;
    auto A0212 = m10 * m22 - m12 * m20;
    auto A0113 = m10 * m31 - m11 * m30;
    auto A0112 = m10 * m21 - m11 * m20;

    auto det = m00 * (m11 * A2323 - m12 * A1323 + m13 * A1223) -
               m01 * (m10 * A2323 - m12 * A0323 + m13 * A0223) +
               m02 * (m10 * A1323 - m11 * A0323 + m13 * A0123) -
               m03 * (m10 * A1223 - m11 * A0223 + m12 * A0123);
    if (det == 0)
        return Zero();

    det = 1 / det;

    auto inverted = Zero();
    inverted.m00  = det * (m11 * A2323 - m12 * A1323 + m13 * A1223);
    inverted.m01  = det * -(m01 * A2323 - m02 * A1323 + m03 * A1223);
    inverted.m02  = det * (m01 * A2313 - m02 * A1313 + m03 * A1213);
    inverted.m03  = det * -(m01 * A2312 - m02 * A1312 + m03 * A1212);
    inverted.m10  = det * -(m10 * A2323 - m12 * A0323 + m13 * A0223);
    inverted.m11  = det * (m00 * A2323 - m02 * A0323 + m03 * A0223);
    inverted.m12  = det * -(m00 * A2313 - m02 * A0313 + m03 * A0213);
    inverted.m13  = det * (m00 * A2312 - m02 * A0312 + m03 * A0212);
    inverted.m20  = det * (m10 * A1323 - m11 * A0323 + m13 * A0123);
    inverted.m21  = det * -(m00 * A1323 - m01 * A0323 + m03 * A0123);
    inverted.m22  = det * (m00 * A1313 - m01 * A0313 + m03 * A0113);
    inverted.m23  = det * -(m00 * A1312 - m01 * A0312 + m03 * A0112);
    inverted.m30  = det * -(m10 * A1223 - m11 * A0223 + m12 * A0123);
    inverted.m31  = det * (m00 * A1223 - m01 * A0223 + m02 * A0123);
    inverted.m32  = det * -(m00 * A1213 - m01 * A0213 + m02 * A0113);
    inverted.m33  = det * (m00 * A1212 - m01 * A0212 + m02 * A0112);
    return inverted;
}
Matrix4x4 Matrix4x4::Transpose() const
{
    auto transposed = Zero();

    transposed.m00 = m00;
    transposed.m01 = m10;
    transposed.m02 = m20;
    transposed.m03 = m30;

    transposed.m10 = m01;
    transposed.m11 = m11;
    transposed.m12 = m21;
    transposed.m13 = m31;

    transposed.m20 = m02;
    transposed.m21 = m12;
    transposed.m22 = m22;
    transposed.m23 = m32;

    transposed.m30 = m03;
    transposed.m31 = m13;
    transposed.m32 = m23;
    transposed.m33 = m33;

    return transposed;
}

Matrix4x4 Matrix4x4::Perspective(float _fov, float _aspect, float _nearZ, float _farZ)
{
    auto top    = _nearZ * (static_cast<float>(M_PI) / 180 * _fov / 2);
    auto bottom = -top;
    auto right  = _aspect * top;
    auto left   = -right;

    auto matrix = Matrix4x4::Zero();
    matrix.m00  = 2 * _nearZ / (right - left);
    matrix.m11  = 2 * _nearZ / (top - bottom);
    matrix.m20  = (right + left) / (right - left);
    matrix.m21  = (top + bottom) / (top - bottom);
    matrix.m22  = (_farZ + _nearZ) / (_farZ - _nearZ);
    matrix.m23  = 1;
    matrix.m32  = -2 * _farZ * _nearZ / (_farZ - _nearZ);
    return matrix;
}

Matrix4x4 Matrix4x4::Orthographic(float _left, float _right, float _bottom, float _top, float _near, float _far)
{
    auto matrix = Matrix4x4::Zero();
    matrix.m00  = 2.0f / (_right - _left);
    matrix.m11  = 2.0f / (_top - _bottom);
    matrix.m22  = 2.0f / (_far - _near);
    matrix.m30  = -(_right + _left) / (_right - _left);
    matrix.m31  = -(_top + _bottom) / (_top - _bottom);
    matrix.m32  = -(_far + _near) / (_far - _near);
    matrix.m33  = 1;
    return matrix;
}

Vector3 Matrix4x4::GetPosition() const
{
    return {m30, m31, m32};
}
