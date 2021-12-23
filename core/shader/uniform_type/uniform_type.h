#ifndef OPENGL_STUDY_UNIFORM_TYPE_H
#define OPENGL_STUDY_UNIFORM_TYPE_H

#include <OpenGL/gl3.h>

enum class UniformType
{
    UNKNOWN,

    INT,

    FLOAT,
    FLOAT_VEC2,
    FLOAT_VEC3,
    FLOAT_VEC4,
    FLOAT_MAT4,

    BOOL,

    SAMPLER_2D,
    SAMPLER_2D_ARRAY,
    SAMPLER_CUBE,
};

namespace UniformTypeUtils
{
    bool        IsTexture(UniformType _uniformType);
    UniformType ConvertUniformType(GLenum _type);
}; // namespace UniformTypeUtils

#endif //OPENGL_STUDY_UNIFORM_TYPE_H
