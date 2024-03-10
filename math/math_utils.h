#ifndef RENDER_ENGINE_MATH_UTILS_H
#define RENDER_ENGINE_MATH_UTILS_H

namespace Math
{
    float Lerp(float _a, float _b, float _t);

    template<typename T>
    int Sign(T _val)
    {
        return (T(0) < _val) - (_val < T(0));
    }
}; // namespace Math

#endif //RENDER_ENGINE_MATH_UTILS_H
