#include "uniform_type.h"

namespace UniformTypeUtils
{
    bool IsTexture(UniformType _uniformType)
    {
        return _uniformType == UniformType::SAMPLER_2D ||
               _uniformType == UniformType::SAMPLER_2D_ARRAY ||
               _uniformType == UniformType::SAMPLER_CUBE;
    }

    UniformType ConvertUniformType(GLenum _type)
    {
        switch (_type)
        {
            case GL_INT:
                return UniformType::INT;

            case GL_FLOAT:
                return UniformType::FLOAT;
            case GL_FLOAT_VEC2:
                return UniformType::FLOAT_VEC2;
            case GL_FLOAT_VEC3:
                return UniformType::FLOAT_VEC3;
            case GL_FLOAT_VEC4:
                return UniformType::FLOAT_VEC4;
            case GL_FLOAT_MAT4:
                return UniformType::FLOAT_MAT4;

            case GL_BOOL:
                return UniformType::BOOL;

            case GL_SAMPLER_2D:
                return UniformType::SAMPLER_2D;
            case GL_SAMPLER_2D_ARRAY:
                return UniformType::SAMPLER_2D_ARRAY;
            case GL_SAMPLER_CUBE:
                return UniformType::SAMPLER_CUBE;

            default:
                return UniformType::UNKNOWN;
        }
    }
} // namespace UniformTypeUtils