#include "property_block.h"
#include "shader/shader.h"
#include "texture/texture.h"


void PropertyBlock::SetTexture(const std::string &_name, std::shared_ptr<Texture> _value)
{
    m_Textures[_name] = std::move(_value);
}

const std::shared_ptr<Texture> PropertyBlock::GetTexture(const std::string &_name) const
{
    return m_Textures.contains(_name) ? m_Textures.at(_name) : nullptr;
}


void PropertyBlock::SetVector(const std::string &_name, const Vector4 &_value)
{
    m_Vectors[_name] = _value;
}

Vector4 PropertyBlock::GetVector(const std::string &_name) const
{
    return m_Vectors.contains(_name) ? m_Vectors.at(_name) : Vector4::Zero();
}


void PropertyBlock::SetFloat(const std::string &_name, float _value)
{
    m_Floats[_name] = _value;
}

float PropertyBlock::GetFloat(const std::string &_name) const
{
    return m_Floats.contains(_name) ? m_Floats.at(_name) : 0;
}


void PropertyBlock::SetMatrix(const std::string &_name, const Matrix4x4 &_value)
{
    m_Matrices[_name] = _value;
}

Matrix4x4 PropertyBlock::GetMatrix(const std::string &_name) const
{
    return m_Matrices.contains(_name) ? m_Matrices.at(_name) : Matrix4x4::Zero();
}