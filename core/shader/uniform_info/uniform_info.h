#ifndef OPENGL_STUDY_UNIFORM_INFO_H
#define OPENGL_STUDY_UNIFORM_INFO_H

#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif
#include <memory>

using namespace std;

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

struct UniformInfo
{
    UniformType Type;
    GLint       Location;
    int         Index;
};

namespace UniformUtils
{
    void        SetUniform(UniformInfo _info, const void *_value);
    bool        IsTexture(UniformType _uniformType);
    UniformType ConvertUniformType(GLenum _type);
}; // namespace UniformUtils

#endif //OPENGL_STUDY_UNIFORM_INFO_H
