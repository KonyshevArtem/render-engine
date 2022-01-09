#include "material.h"
#include "../../math/vector4/vector4.h"
#include "../shader/shader.h"
#include "../texture_2d/texture_2d.h"
#include <utility>

Material::Material(shared_ptr<Shader> _shader) :
    m_Shader(std::move(_shader))
{
}

void Material::SetTexture(const string &_name, shared_ptr<Texture2D> _value)
{
    m_Textures2D[_name]      = std::move(_value);
    m_Vectors4[_name + "ST"] = Vector4(0, 0, 1, 1);
}

const shared_ptr<Texture2D> &Material::GetTexture(const string &_name) const
{
    return m_Textures2D.contains(_name) ? m_Textures2D.at(_name) : Texture2D::Null();
}

void Material::SetVector4(const string &_name, const Vector4 &_value)
{
    m_Vectors4[_name] = _value;
}

const Vector4 &Material::GetVector4(const string &_name) const
{
    return m_Vectors4.contains(_name) ? m_Vectors4.at(_name) : Vector4::Zero();
}

void Material::SetFloat(const string &_name, float _value)
{
    m_Floats[_name] = _value;
}

float Material::GetFloat(const string &_name) const
{
    return m_Floats.contains(_name) ? m_Floats.at(_name) : 0;
}

const shared_ptr<Shader> &Material::GetShader() const
{
    return m_Shader;
}

void Material::TransferUniforms() const
{
    for (const auto &pair: m_Textures2D)
    {
        if (pair.second != nullptr)
            Shader::SetTextureUniform(pair.first, *pair.second);
    }
    for (const auto &pair: m_Vectors4)
        Shader::SetUniform(pair.first, &pair.second);
    for (const auto &pair: m_Floats)
        Shader::SetUniform(pair.first, &pair.second);
}
