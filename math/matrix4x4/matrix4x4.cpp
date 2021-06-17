#include "matrix4x4.h"
#include <cstdio>

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

Matrix4x4 Matrix4x4::Translation(Vector4 translation)
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

Matrix4x4 Matrix4x4::Scale(Vector4 scale)
{
    Matrix4x4 result = Matrix4x4::Identity();
    result.m00       = scale.x;
    result.m11       = scale.y;
    result.m22       = scale.z;
    return result;
}

Matrix4x4 Matrix4x4::TRS(Vector4 translation, Quaternion rotation, Vector4 scale)
{
    return Multiply(Multiply(Translation(translation), Rotation(rotation)), Scale(scale));
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

Matrix4x4 Matrix4x4::Multiply(Matrix4x4 a, Matrix4x4 b)
{
    Matrix4x4 result = Matrix4x4::Zero();
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float sum = 0;
            for (int k = 0; k < 4; ++k)
                sum += a.GetElement(k, j) * b.GetElement(i, k);
            result.SetElement(i, j, sum);
        }
    }
    return result;
}

void Matrix4x4::Print() const
{
    for (int i = 0; i < 4; ++i)
    {
        printf("%f %f %f %f\n",
               this->GetElement(0, i),
               this->GetElement(1, i),
               this->GetElement(2, i),
               this->GetElement(3, i));
    }
}
