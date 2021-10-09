#ifndef OPENGL_STUDY_UNIFORM_TYPE_H
#define OPENGL_STUDY_UNIFORM_TYPE_H

enum class UniformType
{
    UNKNOWN,

    INT,

    FLOAT,
    FLOAT_VEC3,
    FLOAT_VEC4,
    FLOAT_MAT4,

    BOOL,

    SAMPLER_2D,
    SAMPLER_2D_ARRAY,
    SAMPLER_CUBE,
};

class UniformTypeUtils
{
public:
    static bool IsTexture(UniformType _uniformType)
    {
        return _uniformType == UniformType::SAMPLER_2D ||
               _uniformType == UniformType::SAMPLER_2D_ARRAY ||
               _uniformType == UniformType::SAMPLER_CUBE;
    }
};

#endif //OPENGL_STUDY_UNIFORM_TYPE_H
