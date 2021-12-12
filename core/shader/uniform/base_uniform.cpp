#include "base_uniform.h"

#pragma region construction

BaseUniform::BaseUniform(GLint _location, GLenum _type, int _index) :
    m_Location(_location), m_Type(UniformTypeUtils::ConvertUniformType(_type)), m_Index(_index)
{
}

#pragma endregion

#pragma region public methods

void BaseUniform::Set(const void *_value) const
{
    if (_value == nullptr)
        return;

    switch (m_Type)
    {
        case UniformType::UNKNOWN:
            break;
        case UniformType::INT: // NOLINT(bugprone-branch-clone)
        case UniformType::BOOL:
        case UniformType::SAMPLER_2D:
        case UniformType::SAMPLER_2D_ARRAY:
        case UniformType::SAMPLER_CUBE:
            glUniform1i(m_Location, *(static_cast<const GLint *>(_value)));
            break;
        case UniformType::FLOAT:
            glUniform1f(m_Location, *(static_cast<const GLfloat *>(_value)));
            break;
        case UniformType::FLOAT_VEC3:
            glUniform3fv(m_Location, 1, static_cast<const GLfloat *>(_value));
            break;
        case UniformType::FLOAT_VEC4:
            glUniform4fv(m_Location, 1, static_cast<const GLfloat *>(_value));
            break;
        case UniformType::FLOAT_MAT4:
            glUniformMatrix4fv(m_Location, 1, GL_FALSE, static_cast<const GLfloat *>(_value));
            break;
    }
}

int BaseUniform::GetIndex() const
{
    return m_Index;
}

UniformType BaseUniform::GetType() const
{
    return m_Type;
}

#pragma endregion
