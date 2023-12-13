#include "uniform_info.h"
#include "debug.h"

namespace UniformUtils
{
    void SetUniform(UniformInfo _uniform, const void *_value)
    {
        if (_value == nullptr)
            return;

        switch (_uniform.Type)
        {
            case UniformType::UNKNOWN:
                break;
            case UniformType::INT: // NOLINT(bugprone-branch-clone)
            case UniformType::BOOL:
            case UniformType::SAMPLER_2D:
            case UniformType::SAMPLER_2D_ARRAY:
            case UniformType::SAMPLER_CUBE:
                CHECK_GL(glUniform1i(_uniform.Location, *(static_cast<const GLint *>(_value))));
                break;
            case UniformType::FLOAT:
                CHECK_GL(glUniform1f(_uniform.Location, *(static_cast<const GLfloat *>(_value))));
                break;
            case UniformType::FLOAT_VEC2:
                CHECK_GL(glUniform2fv(_uniform.Location, 1, static_cast<const GLfloat *>(_value)));
                break;
            case UniformType::FLOAT_VEC3:
                CHECK_GL(glUniform3fv(_uniform.Location, 1, static_cast<const GLfloat *>(_value)));
                break;
            case UniformType::FLOAT_VEC4:
                CHECK_GL(glUniform4fv(_uniform.Location, 1, static_cast<const GLfloat *>(_value)));
                break;
            case UniformType::FLOAT_MAT4:
                CHECK_GL(glUniformMatrix4fv(_uniform.Location, 1, GL_FALSE, static_cast<const GLfloat *>(_value)));
                break;
        }
    }

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
} // namespace UniformUtils
