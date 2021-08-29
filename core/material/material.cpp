#include "material.h"

#include <utility>

Material::Material(shared_ptr<Shader> _shader)
{
    m_Shader = std::move(_shader);
}

void Material::SetTexture(const string &_name, shared_ptr<Texture> _value)
{
    m_Textures[_name]        = std::move(_value);
    m_Vectors4[_name + "ST"] = Vector4(0, 0, 1, 1);
}

shared_ptr<Texture> Material::GetTexture(const string &_name)
{
    return m_Textures.contains(_name) ? m_Textures[_name] : nullptr;
}

void Material::SetVector4(const string &_name, Vector4 _value)
{
    m_Vectors4[_name] = _value;
}

Vector4 Material::GetVector4(const string &_name)
{
    return m_Vectors4.contains(_name) ? m_Vectors4[_name] : Vector4();
}

void Material::SetFloat(const string &_name, float _value)
{
    m_Floats[_name] = _value;
}

float Material::GetFloat(const string &_name)
{
    return m_Floats.contains(_name) ? m_Floats[_name] : 0;
}
