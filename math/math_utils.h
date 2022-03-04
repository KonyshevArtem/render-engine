#ifndef OPENGL_STUDY_MATH_UTILS_H
#define OPENGL_STUDY_MATH_UTILS_H

namespace Math
{
    float Lerp(float _a, float _b, float _t);

    template<typename T>
    int Sign(T _val)
    {
        return (T(0) < _val) - (_val < T(0));
    }
}; // namespace Math

#endif //OPENGL_STUDY_MATH_UTILS_H
