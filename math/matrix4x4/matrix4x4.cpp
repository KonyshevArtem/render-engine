#include "matrix4x4.h"

Matrix4x4 Matrix4x4::Zero()
{
    Matrix4x4 zero = Matrix4x4();
    zero.m00       = 0;
    zero.m01       = 0;
    zero.m02       = 0;
    zero.m03       = 0;
    zero.m10       = 0;
    zero.m11       = 0;
    zero.m12       = 0;
    zero.m13       = 0;
    zero.m20       = 0;
    zero.m21       = 0;
    zero.m22       = 0;
    zero.m23       = 0;
    zero.m30       = 0;
    zero.m31       = 0;
    zero.m32       = 0;
    zero.m33       = 0;
    return zero;
}

Matrix4x4 Matrix4x4::Identity()
{
    Matrix4x4 identity = Matrix4x4();
    identity.m00       = 1;
    identity.m11       = 1;
    identity.m22       = 1;
    identity.m33       = 1;
    return identity;
}

Matrix4x4 Matrix4x4::Translation(Vector3 translation)
{
    Matrix4x4 translationMatrix = Matrix4x4::Identity();
    translationMatrix.m30       = translation.x;
    translationMatrix.m31       = translation.y;
    translationMatrix.m32       = translation.z;
    return translationMatrix;
}

Matrix4x4 Matrix4x4::Rotation(Quaternion quaternion)
{
    Matrix4x4 result = Matrix4x4::Identity();
    float     x      = quaternion.x;
    float     y      = quaternion.y;
    float     z      = quaternion.z;
    float     w      = quaternion.w;

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

Matrix4x4 Matrix4x4::Scale(Vector3 scale)
{
    Matrix4x4 result = Matrix4x4::Identity();
    result.m00       = scale.x;
    result.m11       = scale.y;
    result.m22       = scale.z;
    return result;
}

Matrix4x4 Matrix4x4::TRS(Vector3 translation, Quaternion rotation, Vector3 scale)
{
    return Translation(translation) * Rotation(rotation) * Scale(scale);
}

float Matrix4x4::GetElement(int column, int row) const
{
    auto *floatPtr = (float *) this;
    return *(floatPtr + 4 * column + row);
}

void Matrix4x4::SetElement(int column, int row, float value)
{
    auto *floatPtr                 = (float *) this;
    *(floatPtr + 4 * column + row) = value;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &matrix) const
{
    Matrix4x4 result = Matrix4x4::Zero();
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float sum = 0;
            for (int k = 0; k < 4; ++k)
                sum += GetElement(k, j) * matrix.GetElement(i, k);
            result.SetElement(i, j, sum);
        }
    }
    return result;
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
    float A2323 = m22 * m33 - m23 * m32;
    float A1323 = m21 * m33 - m23 * m31;
    float A1223 = m21 * m32 - m22 * m31;
    float A0323 = m20 * m33 - m23 * m30;
    float A0223 = m20 * m32 - m22 * m30;
    float A0123 = m20 * m31 - m21 * m30;
    float A2313 = m12 * m33 - m13 * m32;
    float A1313 = m11 * m33 - m13 * m31;
    float A1213 = m11 * m32 - m12 * m31;
    float A2312 = m12 * m23 - m13 * m22;
    float A1312 = m11 * m23 - m13 * m21;
    float A1212 = m11 * m22 - m12 * m21;
    float A0313 = m10 * m33 - m13 * m30;
    float A0213 = m10 * m32 - m12 * m30;
    float A0312 = m10 * m23 - m13 * m20;
    float A0212 = m10 * m22 - m12 * m20;
    float A0113 = m10 * m31 - m11 * m30;
    float A0112 = m10 * m21 - m11 * m20;

    float det = m00 * (m11 * A2323 - m12 * A1323 + m13 * A1223) -
                m01 * (m10 * A2323 - m12 * A0323 + m13 * A0223) +
                m02 * (m10 * A1323 - m11 * A0323 + m13 * A0123) -
                m03 * (m10 * A1223 - m11 * A0223 + m12 * A0123);
    if (det == 0)
        return Zero();

    det = 1 / det;

    Matrix4x4 inverted = Zero();
    inverted.m00       = det * (m11 * A2323 - m12 * A1323 + m13 * A1223);
    inverted.m01       = det * -(m01 * A2323 - m02 * A1323 + m03 * A1223);
    inverted.m02       = det * (m01 * A2313 - m02 * A1313 + m03 * A1213);
    inverted.m03       = det * -(m01 * A2312 - m02 * A1312 + m03 * A1212);
    inverted.m10       = det * -(m10 * A2323 - m12 * A0323 + m13 * A0223);
    inverted.m11       = det * (m00 * A2323 - m02 * A0323 + m03 * A0223);
    inverted.m12       = det * -(m00 * A2313 - m02 * A0313 + m03 * A0213);
    inverted.m13       = det * (m00 * A2312 - m02 * A0312 + m03 * A0212);
    inverted.m20       = det * (m10 * A1323 - m11 * A0323 + m13 * A0123);
    inverted.m21       = det * -(m00 * A1323 - m01 * A0323 + m03 * A0123);
    inverted.m22       = det * (m00 * A1313 - m01 * A0313 + m03 * A0113);
    inverted.m23       = det * -(m00 * A1312 - m01 * A0312 + m03 * A0112);
    inverted.m30       = det * -(m10 * A1223 - m11 * A0223 + m12 * A0123);
    inverted.m31       = det * (m00 * A1223 - m01 * A0223 + m02 * A0123);
    inverted.m32       = det * -(m00 * A1213 - m01 * A0213 + m02 * A0113);
    inverted.m33       = det * (m00 * A1212 - m01 * A0212 + m02 * A0112);
    return inverted;
}
Matrix4x4 Matrix4x4::Transpose() const
{
    Matrix4x4 transposed = Zero();

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
