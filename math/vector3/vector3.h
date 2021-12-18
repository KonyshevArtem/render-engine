#ifndef OPENGL_STUDY_VECTOR3_H
#define OPENGL_STUDY_VECTOR3_H

struct [[nodiscard]] Vector3
{
public:
    float x;
    float y;
    float z;

    Vector3();

    Vector3(float _x, float _y, float _z);

    static inline const Vector3 &One()
    {
        return m_One;
    }

    [[nodiscard]] float Length() const;

    Vector3 Normalize() const;

    Vector3 operator-() const;

    Vector3 operator+(const Vector3 &_vector) const;

    Vector3 operator-(const Vector3 &_vector) const;

    Vector3 operator*(float _value) const;

    static float Dot(const Vector3 &_a, const Vector3 &_b);

    static Vector3 Cross(const Vector3 &_a, const Vector3 &_b);

private:
    static const Vector3 m_One;
};

#endif //OPENGL_STUDY_VECTOR3_H
